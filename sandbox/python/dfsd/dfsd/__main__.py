import sys

from tornado import ioloop as ti, web as tw, httpserver as ths
import wcpan.logger as wl

from . import view


def main(args=None):
    wl.setup([
        'tornado.access',
        'dfsd',
    ])

    loop = ti.IOLoop.instance()

    application = tw.Application([
        (r'^/$', view.Root),
    ])

    server = ths.HTTPServer(application)
    server.listen(8000)

    loop.start()
    loop.close()
    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
