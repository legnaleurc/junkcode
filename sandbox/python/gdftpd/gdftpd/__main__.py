import sys

import tornado.platform.asyncio as tpa

from .server import FtpServer


async def start_server():
    server = FtpServer()
    await server.start('0.0.0.0', 2121)


def main(args=None):
    if args is None:
        args = sys.argv

    loop = tpa.AsyncIOMainLoop()
    loop.install()

    loop.add_callback(start_server)

    loop.start()
    loop.close()

    return 0


rv = main()
sys.exit(rv)
