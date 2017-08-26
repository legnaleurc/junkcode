import tornado.ioloop

from .network import FTPServer
from .provider import LocalFileSystemProvider


if __name__ == '__main__':
    PORT = 2121
    lfsp = LocalFileSystemProvider('.')
    server = FTPServer(lfsp)
    server.listen(PORT)
    print("Starting server on port {}".format(PORT))
    tornado.ioloop.IOLoop.instance().start()
