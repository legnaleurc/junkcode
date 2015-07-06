#! /usr/bin/env python3

import sys

from tornado import ioloop, gen, web, log

from .api import Session


class RootHandler(web.RequestHandler):

    def get(self):
        a = self.request.arguments
        print(a)

    def post(self):
        pass

    def head(self):
        pass

    def put(self):
        pass

    def delete(self):
        pass


@gen.coroutine
def alpha():
    application = web.Application([
        (r'.*', RootHandler),
    ])
    application.listen(8000)

    session = Session()
    a = yield session.login()
    #print(a)
    with open('/tmp/a.html', 'wb') as fout:
        fout.write(a)


def main(args=None):
    if args is None:
        args = sys.argv

    log.enable_pretty_logging()

    main_loop = ioloop.IOLoop.instance()
    main_loop.run_sync(alpha)

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
