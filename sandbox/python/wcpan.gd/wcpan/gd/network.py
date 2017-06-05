import json
import time
import urllib.parse as up

from tornado import httpclient as thc, httputil as thu


class Network(object):

    def __init__(self, access_token):
        self._access_token = access_token
        self._http = thc.AsyncHTTPClient()
        self._headers = {
            'Authorization': 'Bearer {0}'.format(self._access_token),
        }

    async def get(self, path, args=None, headers=None, body=None,
                  consumer=None):
        while True:
            rv = await self._do_request('GET', path, args, headers, body,
                                        consumer)
            rv = self._maybe_backoff(rv)
            if rv:
                return rv

    async def post(self, path, args=None, headers=None, body=None,
                   consumer=None):
        while True:
            rv = await self._do_request('POST', path, args, headers, body,
                                        consumer)
            rv = self._maybe_backoff(rv)
            if rv:
                return rv

    async def put(self, path, args=None, headers=None, body=None,
                  consumer=None):
        while True:
            rv = await self._do_request('PUT', path, args, headers, body,
                                        consumer)
            rv = self._maybe_backoff(rv)
            if rv:
                return rv

    async def _do_request(self, method, path, args, headers, body, consumer):
        # TODO wait for backoff timeout

        headers = self._prepare_headers(headers)
        if args is not None:
            path = thu.url_concat(path, args)

        args = {
            'url': path,
            'method': method,
            'headers': headers,
            'body': body,
            'body_producer': body_producer,
            'streaming_callback': consumer,
        }
        if body is not None:
            if callable(body):
                args['body_producer'] = body
            else:
                args['body'] = body
        request = thc.HTTPRequest(**args)

        rv = await self._http.fetch(request, raise_error=False)
        return Response(rv)

    def _prepare_headers(self, headers):
        h = dict(self._headers)
        if headers is not None:
            h.update(headers)
        h = {k: v if isinstance(v, (bytes, str)) or v is None else str(v)
             for k, v in h.items()}
        return h

    def _maybe_backoff(self, response):
        if response.status != '403':
            return response
        msg = response.json_
        domain = msg['error']['errors'][0]['domain']
        if domain != 'usageLimits':
            return response
        # TODO implement backoff strategy
        raise Exception('API rate limit')


class Response(object):

    def __init__(self, response):
        self._response = response
        self._status = str(self._response.code)
        self._parsed_json = False
        self._json = None

    @property
    def status(self):
        return self._status

    @property
    def json_(self):
        if not self._parsed_json:
            rv = self._response.body.decode('utf-8')
            rv = json.loads(rv)
            self._json = rv
            self._parsed_json = True
        return self._json

    def get_header(self, key):
        h = self._response.headers.get_list(key)
        return None if not h else h[0]
