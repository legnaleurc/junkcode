import functools
import signal

from tornado import (ioloop as ti,
                     web as tw,
                     httpserver as ths)
import wcpan.listen as wls
import wcpan.logger as wlg


def main(args=None):
    args = parse_args(args)
    wlg.setup((
        'tornado.access',
        'tornado.application',
        'tornado.general',
        'requests.packages.urllib3.connectionpool',
    ))

    loop = ti.IOLoop.instance()

    application = tw.Application([
        (r'*', Handler),
    ])
    server = ths.HTTPServer(application)

    signal.signal(signal.SIGINT, functools.partial(shutdown, loop=loop))

    with create_sockets(['/tmp/proxy.socket']) as sockets:
        server.add_sockets(sockets)
        loop.start()
        loop.close()

    return 0


def shutdown(signum, frame, loop):
    loop.stop()
