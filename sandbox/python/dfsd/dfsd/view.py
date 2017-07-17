import xml.etree.ElementTree as ET

from tornado import web as tw

import wcpan.logger as wl


NS = {
    'D': 'DAV:',
}


class Root(tw.RequestHandler):

    SUPPORTED_METHODS = tw.RequestHandler.SUPPORTED_METHODS + ('MKCOL', 'PROPFIND', 'PROPPATCH', 'ORDERPATCH', 'COPY', 'MOVE', 'TRACE', 'REPORT', 'LOCK', 'UNLOCK')

    def options(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

        self.set_header('Allow', 'OPTIONS, GET, HEAD, POST, DELETE, PATCH, PUT, MKCOL, PROPFIND, PROPPATCH, ORDERPATCH, COPY, MOVE, TRACE, REPORT, LOCK, UNLOCK')
        self.set_header('DAV', '1, 2')

    def get(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def head(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def post(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def delete(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def patch(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def put(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def mkcol(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def propfind(self):
        depth = self.request.headers['Depth']
        data = self.request.body
        wl.DEBUG('dfsd') << data
        root = ET.fromstring(data)
        prop = root.find('./D:prop', NS)
        for key in prop:
            wl.DEBUG('dfsd') << key.tag

    def proppatch(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def orderpatch(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def lock(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def unlock(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def copy(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body

    def move(self):
        wl.DEBUG('dfsd') << self.request.headers
        wl.DEBUG('dfsd') << self.request.body
