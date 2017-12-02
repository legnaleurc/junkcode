import functools as ft
import os.path as op
import sys

import tornado.platform.asyncio as tpa
import wcpan.drive.google as wdg

from .server import FtpServer
from .fs import GoogleDrivePathIO


async def setup():
    path = op.expanduser('~/.cache/wcpan/drive/google')
    drive = wdg.Drive(path)
    await drive.initialize()

    factory = ft.partial(GoogleDrivePathIO, drive=drive)

    server = FtpServer(path_io_factory=factory)
    await server.start('0.0.0.0', 2121)

    print('ready')


def main(args=None):
    if args is None:
        args = sys.argv

    loop = tpa.AsyncIOMainLoop()
    loop.install()

    loop.add_callback(setup)

    loop.start()
    loop.close()

    return 0


rv = main()
sys.exit(rv)
