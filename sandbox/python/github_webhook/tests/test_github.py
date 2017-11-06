import unittest as ut

from tornado import testing as tt

from triage_bot import github as tbgh


class TestGitHub(tt.AsyncTestCase):

    @tt.gen_test
    def testFetchTriagedIssues(self):
        rv = await fetch_triaged_issues('legnaleurc', 'junkcode')
        print(rv)
