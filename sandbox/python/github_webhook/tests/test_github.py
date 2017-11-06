import unittest as ut

from tornado import testing as tt

from triage_bot import github as tbgh


class TestGitHub(tt.AsyncTestCase):

    @tt.gen_test
    def testFetchTriagedIssues(self):
        ghapi = tbgh.GitHubAPI('')
        issues = yield ghapi.fetch_issues('adsbypasser', 'adsbypasser')
        print(len(issues))
        #for issue in issues:
