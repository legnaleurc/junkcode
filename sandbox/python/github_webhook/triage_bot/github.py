from tornado import httpclient as thc

GITHUB_API_URL = 'https://api.github.com'


async def fetch_triaged_issues(owner, repo):
    curl = thc.AsyncHTTPClient()
    url = GITHUB_API_URL + '/repos/{owner}/{repo}/issues'.format(owner=owner, repo=repo)
    r = thc.HTTPRequest(url, method='GET', headers={
        'Accept': 'application/vnd.github.v3+json',
    })
    rv = await curl.fetch(r)
    rv = rv.body
    return rv
