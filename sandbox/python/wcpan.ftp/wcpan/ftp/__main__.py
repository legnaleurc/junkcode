import tornado.ioloop

from .network import FTPServer


if __name__ == '__main__':
    PORT = 2121
    server = FTPServer(".")
    server.listen(PORT)
    print("Starting server on port {}".format(PORT))
    tornado.ioloop.IOLoop.instance().start()
