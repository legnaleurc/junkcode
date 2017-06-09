import json
import math
import random
import urllib.parse as up

from tornado import httpclient as thc, httputil as thu, gen as tg
from wcpan.logger import DEBUG, EXCEPTION

from .util import GoogleDriveError


BACKOFF_FACTOR = 2
BACKOFF_STATUSES = ('403', '500', '502', '503', '504')


class Network(object):

    def __init__(self):
        self._access_token = None
        self._http = thc.AsyncHTTPClient()
        self._headers = {
        }
        self._backoff_level = 0

    def set_access_token(self, token):
        self._access_token = token

    async def fetch(self, method, path, args=None, headers=None, body=None,
                    consumer=None, timeout=None):
        await self._maybe_backoff()
        return await self._do_request(method, path, args, headers, body,
                                      consumer, timeout)

    async def _do_request(self, method, path, args, headers, body, consumer,
                          timeout):
        headers = self._prepare_headers(headers)
        if args is not None:
            path = thu.url_concat(path, args)

        args = {
            'url': path,
            'method': method,
            'headers': headers,
        }
        if body is not None:
            if callable(body):
                args['body_producer'] = body
            else:
                args['body'] = body
        if consumer is not None:
            args['streaming_callback'] = consumer
        if timeout is not None:
            args['request_timeout'] = timeout

        request = thc.HTTPRequest(**args)
        rv = await self._http.fetch(request, raise_error=False)
        rv = Response(rv)
        rv = self._handle_status(rv)
        return rv

    def _prepare_headers(self, headers):
        h = {
            'Authorization': 'Bearer {0}'.format(self._access_token),
        }
        if headers is not None:
            h.update(headers)
        h = {k: v if isinstance(v, (bytes, str)) or v is None else str(v)
             for k, v in h.items()}
        return h

    def _handle_status(self, response):
        if backoff_needed(response):
            self._increase_backoff_level()
        else:
            self._reset_backoff_level()

        # normal response
        if response.status[0] in ('1', '2', '3'):
            return response

        # otherwise it is an error
        raise NetworkError(response)

    def _increase_backoff_level(self):
        self._backoff_level = self._backoff_level + 1

    def _reset_backoff_level(self):
        self._backoff_level = 0

    async def _maybe_backoff(self):
        if self._backoff_level <= 0:
            return
        seed = random.random()
        power = 2 ** self._backoff_level
        s_delay = math.floor(seed * power * BACKOFF_FACTOR)
        s_delay = min(100, s_delay)
        DEBUG('wcpan.gd') << 'backoff for' << s_delay
        await tg.sleep(s_delay)


class Request(object):

    def __init__(self, request):
        self._request = request

    @property
    def uri(self):
        return self._request.url

    @property
    def method(self):
        return self._request.method

    @property
    def headers(self):
        return self._request.headers


class Response(object):

    def __init__(self, response):
        self._response = response
        self._request = Request(response.request)
        self._status = str(self._response.code)
        self._parsed_json = False
        self._json = None

    @property
    def status(self):
        return self._status

    @property
    def reason(self):
        return self._response.reason

    @property
    def json_(self):
        if not self._parsed_json:
            rv = self._response.body
            if rv:
                rv = rv.decode('utf-8')
                try:
                    rv = json.loads(rv)
                except ValueError as e:
                    EXCEPTION('wcpan.gd') << rv
                    rv = None
            self._json = rv
            self._parsed_json = True
        return self._json

    @property
    def request(self):
        return self._request

    @property
    def error(self):
        return self._response.error

    def get_header(self, key):
        h = self._response.headers.get_list(key)
        return None if not h else h[0]


class NetworkError(GoogleDriveError):

    def __init__(self, response):
        self._response = response
        self._message = '{0} {1} - {2}'.format(self.status,
                                               self._response.reason,
                                               self.json_)

    def __str__(self):
        return self._message

    @property
    def status(self):
        return self._response.status

    @property
    def fatal(self):
        return not backoff_needed(self._response)

    @property
    def json_(self):
        return self._response.json_

    @property
    def error(self):
        return self._response.error


def backoff_needed(response):
    if response.status not in BACKOFF_STATUSES:
        return False

    # if it is not a rate limit error, it could be handled immediately
    if response.status == '403':
        msg = response.json_
        domain = msg['error']['errors'][0]['domain']
        if domain != 'usageLimits':
            return False

    return True
