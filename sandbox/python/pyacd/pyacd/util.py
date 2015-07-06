import urllib.parse as up
from http import cookies, cookiejar
import datetime as dt

from tornado import httpclient, gen, httputil


#@gen.coroutine
#def get(url, query):
    #client = httpclient.AsyncHTTPClient()
    #url = httputil.url_concat(url, query)
    #response = yield client.fetch(url)
    #print(response)
    #return response.body

#@gen.coroutine
#def post(url, query):
    #client = httpclient.AsyncHTTPClient()
    #query = up.urlencode(query)
    #request = httpclient.HTTPRequest(url, method='POST', body=query)
    #response = yield client.fetch(request)
    #return response.body


class HTTPSession(object):

    def __init__(self):
        self._cookies = cookies.SimpleCookie()
        #self._cookies = cookiejar.CookieJar()

    @gen.coroutine
    def get(self, url, query):
        client = httpclient.AsyncHTTPClient()
        url = httputil.url_concat(url, query)
        request = httpclient.HTTPRequest(url)
        self._setup_cookies(request)
        response = yield client.fetch(request)
        self._update_cookies(response)
        return response.body

    @gen.coroutine
    def post(self, url, query):
        client = httpclient.AsyncHTTPClient()
        query = up.urlencode(query)
        request = httpclient.HTTPRequest(url, method='POST', body=query)
        self._setup_cookies(request)
        response = yield client.fetch(request)
        self._update_cookies(response)
        return response.body

    def _update_cookies(self, response):
        if 'Set-Cookie' in response.headers:
            a = response.headers.get_list('Set-Cookie')
            for b in a:
                self._cookies.load(b)

    def _strip_cookies(self):
        new_cookies = cookies.SimpleCookie()
        now = dt.datetime.utcnow()
        for key, cookie in self._cookies.items():
            expires = cookie['expires']
            if expires:
                expires = dt.datetime.strptime(expires, '%a, %d-%b-%Y %H:%M:%S %Z')
                if now <= expires:
                    new_cookies.load(str(cookie))
            else:
                new_cookies.load(str(cookie))
        self._cookies = new_cookies

    def _setup_cookies(self, request):
        self._strip_cookies()
        cookie = '; '.join('{0}={1}'.format(key, cookie.value) for key, cookie in self._cookies.items())
        request.headers.add('Cookie', cookie)
