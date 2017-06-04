import json
import urllib.parse as up

from tornado import ioloop as ti, httpclient as thc


class GoogleOAuth2(object):

    def __init__(self, auth_uri, token_uri, client_id, client_secret, scope):
        self._auth_uri = auth_uri
        self._token_uri = token_uri
        self._client_id = client_id
        self._client_secret = client_secret
        self._scope = scope
        self._http = thc.AsyncHTTPClient()

    def get_auth_url(self, redirect_uri):
        args = {
            'client_id': self._client_id,
            'redirect_uri': redirect_uri,
            'scope': self._scope,
            'access_type': 'offline',
            'response_type': 'code',
        }
        rv = up.urlencode(args)
        return '{0}?{1}'.format(self._auth_uri, rv)

    async def exchange(self, code):
        body = {
            'client_id': self._client_id,
            'code': code,
            'scope': self._scope,
            'client_secret': self._client_secret,
            'grant_type': 'authorization_code',
        }
        body = up.urlencode(body)
        args = {
            'url': self._token_uri,
            'method': 'POST',
            'headers': {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            'body': body,
        }
        r = thc.HTTPRequest(**args)
        rv = await self._http.fetch(r)
        body = rv.body
        body.decode('utf-8')
        print(body)


async def main(args=None):
    with open('client_secret.json', 'r') as fin:
        rv = json.load(fin)

    installed = rv['installed']
    args = {
        'auth_uri': installed['auth_uri'],
        'token_uri': installed['token_uri'],
        'client_id': installed['client_id'],
        'client_secret': installed['client_secret'],
        'scope': 'https://www.googleapis.com/auth/drive',
    }
    auth = GoogleOAuth2(**args)
    redirect_uri = installed['redirect_uris'][0]
    url = auth.get_auth_url(redirect_uri)
    print(url)
    code = input('code: ')
    await auth.exchange(code)


main_loop = ti.IOLoop.instance()
main_loop.run_sync(main)
