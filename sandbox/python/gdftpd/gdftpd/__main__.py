import functools as ft
import logging
import os.path as op
import sys

import tornado.ioloop as ti
import wcpan.drive.google as wdg
import wcpan.logger as wl

from .server import FtpServer
from .fs import GoogleDrivePathIO


async def setup():
    wl.setup([
        'aioftp',
        'wcpan.drive.google',
    ])

    path = op.expanduser('~/.cache/wcpan/drive/google')
    drive = wdg.Drive(path)
    await drive.initialize()

    factory = ft.partial(GoogleDrivePathIO, drive=drive)

    server = FtpServer(path_io_factory=factory)
    await server.start('0.0.0.0', 2121)


def main(args=None):
    if args is None:
        args = sys.argv

    loop = ti.IOLoop.instance()

    loop.add_callback(setup)

    loop.start()
    loop.close()

    return 0


rv = main()
sys.exit(rv)
