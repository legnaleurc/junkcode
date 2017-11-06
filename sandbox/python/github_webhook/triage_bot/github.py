import json

from tornado import httpclient as thc


GITHUB_API_URL = 'https://api.github.com'
USER_AGENT = 'AdsBypasser Bot'


class GitHubAPI(object):

    def __init__(self, token):
        self._token = token

    async def fetch_issues(self, owner, repo):
        curl = thc.AsyncHTTPClient()
        url = GITHUB_API_URL + '/repos/{owner}/{repo}/issues'.format(owner=owner, repo=repo)
        r = thc.HTTPRequest(url, method='GET', headers={
            'Accept': 'application/vnd.github.v3+json',
            #'Authorization': 'token {}'.format(self._token),
            'User-Agent': USER_AGENT,
        })
        rv = await curl.fetch(r)
        print(rv.headers)
        rv = rv.body
        rv = rv.decode('utf-8')
        rv = json.loads(rv)
        return rv
