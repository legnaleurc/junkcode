import json
import urllib.parse as up

from tornado import ioloop as ti, httpclient as thc


class GoogleOAuth2(object):

    def __init__(self, auth_uri, token_uri, client_id, client_secret, scope, redirect_uri):
        self._auth_uri = auth_uri
        self._token_uri = token_uri
        self._client_id = client_id
        self._client_secret = client_secret
        self._scope = scope
        self._redirect_uri = redirect_uri
        self._http = thc.AsyncHTTPClient()

    def get_auth_url(self, ):
        args = {
            'client_id': self._client_id,
            'redirect_uri': self._redirect_uri,
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
            'redirect_uri': self._redirect_uri,
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
        body = body.decode('utf-8')
        body = json.loads(body)
        return body


async def main(args=None):
    with open('client_secret.json', 'r') as fin:
        rv = json.load(fin)

    installed = rv['installed']
    redirect_uri = installed['redirect_uris'][0]
    args = {
        'auth_uri': installed['auth_uri'],
        'token_uri': installed['token_uri'],
        'client_id': installed['client_id'],
        'client_secret': installed['client_secret'],
        'scope': 'https://www.googleapis.com/auth/drive',
        'redirect_uri': redirect_uri,
    }
    auth = GoogleOAuth2(**args)
    url = auth.get_auth_url()
    print(url)
    code = input('code: ')
    oauth_token = await auth.exchange(code)
    print(oauth_token)


main_loop = ti.IOLoop.instance()
main_loop.run_sync(main)
