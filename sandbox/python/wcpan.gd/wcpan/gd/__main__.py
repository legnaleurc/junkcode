import functools as ft
import hashlib
import os
import os.path as op
import pathlib as pl
import sys

from tornado import ioloop as ti, locks as tl, gen as tg
import wcpan.logger as wl

from .drive import Drive
from .util import stream_md5sum
from .network import NetworkError


def traverse_node(drive, node, level):
    if node.is_root:
        print_node('/', level)
    else:
        print_node(node.name, level)

    if node.is_folder:
        children = drive.get_children_by_id(node.id_)
        for child in children:
            traverse_node(drive, child, level + 1)


def print_node(name, level):
    level = ' ' * level
    print(level + name)


async def verify_upload(drive, local_path, remote_node):
    if local_path.is_dir():
        await verify_upload_directory(drive, local_path, remote_node)
    else:
        await verify_upload_file(drive, local_path, remote_node)


async def verify_upload_directory(drive, local_path, remote_node):
    dir_name = local_path.name

    child_node = drive.get_child_by_id(remote_node.id_, dir_name)
    if not child_node:
        print('E not found : {0}'.format(local_path))
        return
    if not child_node.is_folder:
        print('E should be folder : {0}'.format(local_path))
        return

    for child_path in local_path.iterdir():
        await verify_upload(drive, child_path, child_node)


async def verify_upload_file(drive, local_path, remote_node):
    file_name = local_path.name
    remote_path = drive.get_path_by_id(remote_node.id_)
    remote_path = pl.Path(remote_path, file_name)

    child_node = drive.get_child_by_id(remote_node.id_, file_name)

    if not child_node:
        print('E not found : {0}'.format(local_path))
        return
    if child_node.is_folder:
        print('E should be file : {0}'.format(local_path))
        return
    if not child_node.available:
        print('E trashed : {0}'.format(local_path))
        return

    with open(local_path, 'rb') as fin:
        local_md5 = stream_md5sum(local_path)
    if local_md5 != child_node.md5:
        print('E md5 mismatch : {0}'.format(local_path))
        return

    print('ok : {0}'.format(local_path))


class UploadQueue(object):

    def __init__(self):
        self._busy = 0
        self._max = 8
        self._lock = tl.Condition()
        self._final = tl.Condition()

    def push(self, runnable):
        loop = ti.IOLoop.current()
        fn = ft.partial(self._do_push, runnable)
        loop.add_callback(fn)

    async def wait_for_complete(self):
        await self._final.wait()

    async def _do_push(self, runnable):
        async with RunnableRecycler(self):
            await runnable()

    async def _wait_for_idle(self):
        while True:
            if self._busy < self._max:
                self._busy = self._busy + 1
                return

            await self._lock.wait()

    def _done(self):
        self._busy = self._busy - 1
        self._lock.notify()
        if self._busy <= 0:
            self._final.notify()


class RunnableRecycler(object):

    def __init__(self, queue_):
        self._queue = queue_

    async def __aenter__(self):
        await self._queue._wait_for_idle()

    async def __aexit__(self, *args, **kwargs):
        self._queue._done()


async def upload(queue_, drive, local_path, parent_node):
    wl.INFO('wcpan.gd') << 'uploading' << local_path
    if op.isdir(local_path):
        rv = await retry_create_folder(queue_, drive, local_path, parent_node)
    else:
        rv = await retry_upload_file(drive, local_path, parent_node)
    return rv


'''
def generate_upload_tasks(drive, local_path, parent_node):
    INFO('wcpan.gd') << 'uploading' << local_path
    if op.isdir(local_path):
        rv = await drive.create_folder(local_path, parent_node)
        for child_path in os.listdir(local_path):
            child_path = op.join(local_path, child_path)
            fn = ft.partial(upload, queue_, drive, child_path, rv)
            queue_.push(fn)
    else:
        fn = ft.partial(drive.upload_file, local_path, parent_node)
        yield fn
'''


async def retry_upload_file(drive, local_path, parent_node):
    while True:
        try:
            rv = await drive.upload_file(local_path, parent_node)
            break
        except NetworkError as e:
            wl.EXCEPTION('wcpan.gd', e)
            if e.fatal or e.status != '401':
                raise
    return rv


async def retry_create_folder(queue_, drive, local_path, parent_node):
    while True:
        try:
            rv = await drive.create_folder(local_path, parent_node)
            break
        except NetworkError as e:
            wl.EXCEPTION('wcpan.gd', e)
            if e.fatal or e.status != '401':
                raise

    for child_path in os.listdir(local_path):
        child_path = op.join(local_path, child_path)
        fn = ft.partial(upload, queue_, drive, child_path, rv)
        wl.DEBUG('queue') << child_path << rv.name
        queue_.push(fn)

    return rv


async def main(args=None):
    if args is None:
        args = sys.argv

    wl.setup((
        'wcpan.gd',
    ), '/tmp/wcpan.gd.log')

    path = op.expanduser('~/.cache/wcpan/gd')
    drive = Drive(path)
    drive.initialize()
    await drive.sync()

    local_path = args[1]
    remote_path = args[2]

    queue_ = UploadQueue()

    node = drive.get_node_by_path(remote_path)
    fn = ft.partial(upload, queue_, drive, local_path, node)
    queue_.push(fn)
    await tg.sleep(1)
    await queue_.wait_for_complete()

    return 0


main_loop = ti.IOLoop.instance()
exit_code = main_loop.run_sync(main)
main_loop.close()
sys.exit(exit_code)
