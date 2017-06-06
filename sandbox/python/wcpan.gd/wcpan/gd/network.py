import json
import time
import urllib.parse as up

from tornado import httpclient as thc, httputil as thu

from .util import GoogleDriveError


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
            return self._handle_status(rv)

    async def post(self, path, args=None, headers=None, body=None,
                   consumer=None):
        while True:
            rv = await self._do_request('POST', path, args, headers, body,
                                        consumer)
            return self._handle_status(rv)

    async def put(self, path, args=None, headers=None, body=None,
                  consumer=None):
        while True:
            rv = await self._do_request('PUT', path, args, headers, body,
                                        consumer)
            return self._handle_status(rv)

    async def _do_request(self, method, path, args, headers, body, consumer):
        # TODO wait for backoff timeout

        headers = self._prepare_headers(headers)
        if args is not None:
            path = thu.url_concat(path, args)

        args = {
            'url': path,
            'method': method,
            'headers': headers,
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

    def _handle_status(self, response):
        # normal response
        if response.status[0] in ('1', '2', '3'):
            return response

        # could be handled immediately
        if response.status not in ('403', '500', '502', '503', '504'):
            raise NetworkError(response)

        # if it is not a rate limit error, it could be handled immediately
        if response.status == '403':
            msg = response.json_
            domain = msg['error']['errors'][0]['domain']
            if domain != 'usageLimits':
                raise NetworkError(response)

        self._increase_backoff_level()
        raise NetworkError(response)

    def _increase_backoff_level(self):
        # TODO implement backoff strategy
        pass


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


class NetworkError(GoogleDriveError):

    def __init__(self, response):
        self._response = response

    def __str__(self):
        return 'connection error: ' + self._response.reason
