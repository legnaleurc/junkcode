import sys

from tornado import ioloop as ti, web as tw, httpserver as ths
import wcpan.listen as wls
import wcpan.logger as wlg

from . import api


def main(args=None):
    if args is None:
        args = sys.argv

    wlg.setup([
        'triage_bot',
    ])

    application = tw.Application([
        (r'/api/v1/.*', api.IndexHandler),
    ])

    server = ths.HTTPServer(application)

    loop = ti.IOLoop.instance()

    opts = [8000]

    with wls.create_sockets(opts) as sockets:
        server.add_sockets(sockets)
        loop.start()
        loop.close()

    return 0
