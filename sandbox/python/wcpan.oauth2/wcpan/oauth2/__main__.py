import json
import urllib.parse as up

from tornado import ioloop as ti


class GoogleOAuth2(object):

    def __init__(self, auth_uri, token_uri, client_id, client_secret):
        self._auth_uri = auth_uri
        self._token_uri = token_uri
        self._client_id = client_id
        self._client_secret = client_secret

    def get_auth_url(self, redirect_uri, scope):
        args = {
            'client_id': self._client_id,
            'redirect_uri': redirect_uri,
            'scope': scope,
            'access_type': 'offline',
            'response_type': 'code',
        }
        rv = up.urlencode(args)
        return '{0}?{1}'.format(self._auth_uri, rv)


def main(args=None):
    with open('client_secret.json', 'r') as fin:
        rv = json.load(fin)

    installed = rv['installed']
    args = {
        'auth_uri': installed['auth_uri'],
        'token_uri': installed['token_uri'],
        'client_id': installed['client_id'],
        'client_secret': installed['client_secret'],
    }
    auth = GoogleOAuth2(**args)
    scope = 'https://www.googleapis.com/auth/drive'
    url = auth.get_auth_url(installed['redirect_uris'][0], scope)
    print(url)


main_loop = ti.IOLoop.instance()
main_loop.run_sync(main)
