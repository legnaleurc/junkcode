import json
import time
import urllib.parse as up


class Network(object):

    def __init__(self, http):
        self._http = http

    def get(self, path, args=None):
        while True:
            rv = self._get(path, args)
            if rv.status[0] == '2':
                return rv
            if rv.status == '403':
                # FIXME blocking thread, backoff strategy
                time.sleep(5)
                continue
            raise Exception('request error')

    def _get(self, path, args):
        if args is not None:
            args = up.urlencode(args)
            path = '{0}?{1}'.format(path, args)
        response, content = self._http.request(path, 'GET')
        return Response(response, content)


class Response(object):

    def __init__(self, response, content):
        self._response = response
        self._content = content

    @property
    def status(self):
        return self._response['status']

    @property
    def json_(self):
        decoded = self._content.decode('utf-8')
        rv = json.loads(decoded)
        return rv
