import asyncio as aio
import contextlib as cl
import functools as ft

import wcpan.drive.google as wdg

from .api import ffi, lib


def sync(fn):
    @ft.wraps(fn)
    def wrapper(*args, **kwargs):
        loop = aio.get_event_loop()
        f = fn(*args, **kwargs)
        return loop.run_until_complete(f)
    return wrapper


@ffi.def_extern()
@sync
async def drive_read(opaque, buf, buf_size):
    data = ffi.from_handle(opaque)
    drive = data['drive']
    node = data['node']
    stream = data['stream']

    chunk = await stream.read(buf_size)
    ffi.memmove(buf, chunk, len(chunk))
    return len(chunk)


@ffi.def_extern()
@sync
async def drive_seek(opaque, offset, whence):
    data = ffi.from_handle(opaque)
    drive = data['drive']
    node = data['node']
    stream = data['stream']

    if whence == 0x10000:
        return node.size

    await stream.seek(offset)
    print('seeked', offset)
    return offset


@cl.contextmanager
def drive_open():
    loop = aio.get_event_loop()
    drive = wdg.Drive()
    loop.run_until_complete(drive.__aenter__())
    try:
        yield drive
    finally:
        loop.run_until_complete(drive.__aexit__(None, None, None))


def main():
    loop = aio.get_event_loop()
    with drive_open() as drive:
        f = drive.get_node_by_path('/tmp/0511star910/star910-5.mp4')
        node = loop.run_until_complete(f)
        f = drive.download(node)
        stream = loop.run_until_complete(f)
        data = {
            'drive': drive,
            'node': node,
            'stream': stream,
        }
        c_data = ffi.new_handle(data)
        path = f'/tmp/{node.name}_%0d.png'.encode('utf-8')
        handle = lib.screenshot_new(c_data, lib.drive_read, lib.drive_seek)
        if not handle:
            return 1
        try:
            while lib.screenshot_has_next(handle):
                lib.screenshot_slice(handle, path)
        finally:
            lib.screenshot_delete(handle)
    return 0


main()
