import json
import time
import urllib.parse as up

from tornado import httpclient as thc


class Network(object):

    def __init__(self, access_token):
        self._access_token = access_token
        self._http = thc.AsyncHTTPClient()
        self._headers = {
            'Authorization': 'Bearer {0}'.format(self._access_token),
        }

    async def get(self, path, args=None, headers=None):
        while True:
            rv = await self._do_request('GET', path, headers, args)
            if rv.status[0] == '2':
                return rv
            if rv.status == '403':
                # FIXME blocking thread, backoff strategy
                time.sleep(5)
                continue
            raise Exception('request error')

    async def _do_request(self, method, path, headers, args):
        headers = self._prepare_headers(headers)
        if args is not None:
            args = up.urlencode(args)
            path = '{0}?{1}'.format(path, args)

        args = {
            'url': path,
            'method': method,
            'headers': headers,
        }
        request = thc.HTTPRequest(**args)

        rv = await self._http.fetch(request)
        return Response(rv)

    def _prepare_headers(self, headers):
        h = dict(self._headers)
        if headers is not None:
            h.update(headers)
        return h


class Response(object):

    def __init__(self, response):
        self._response = response

    @property
    def status(self):
        return str(self._response.code)

    @property
    def json_(self):
        decoded = self._response.body.decode('utf-8')
        rv = json.loads(decoded)
        return rv
