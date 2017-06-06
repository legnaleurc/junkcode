import hashlib
import os
import os.path as op
import pathlib as pl

from tornado import ioloop as ti

from .drive import Drive


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

    local_md5 = md5sum(local_path)
    if local_md5 != child_node.md5:
        print('E md5 mismatch : {0}'.format(local_path))
        return

    print('ok : {0}'.format(local_path))


def md5sum(local_path):
    hasher = hashlib.md5()
    with open(local_path, 'rb') as fin:
        while True:
            chunk = fin.read(65536)
            if not chunk:
                break
            hasher.update(chunk)
    return hasher.hexdigest()


async def main(args=None):
    path = op.expanduser('~/.cache/wcpan/gd')
    drive = Drive(path)
    drive.initialize()
    await drive.sync()

    local_path = pl.Path('/mnt/Amazon Drive Downloads/tmp')
    remote_node = drive.get_node_by_path('/Backup/L/Game')
    await verify_upload(drive, local_path, remote_node)

    return 0


main_loop = ti.IOLoop.instance()
main_loop.run_sync(main)
main_loop.close()
