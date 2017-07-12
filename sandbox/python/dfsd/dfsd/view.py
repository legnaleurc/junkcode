from tornado import web as tw

import wcpan.logger as wl


class Root(tw.RequestHandler):

    SUPPORTED_METHODS = tw.RequestHandler.SUPPORTED_METHODS + ('MKCOL', 'PROPFIND', 'PROPPATCH', 'ORDERPATCH', 'LOCK', 'UNLOCK', 'COPY', 'MOVE')

    def get(self):
        pass

    def options(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

        self.set_header('Allow', 'OPTIONS, GET, HEAD, POST, DELETE, PATCH, PUT')
        self.set_header('Allow', 'MKCOL, PROPFIND, PROPPATCH, ORDERPATCH, LOCK, UNLOCK, COPY, MOVE')
        self.set_header('DAV', '1, 2, ordered-collections')

    def propfind(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body
