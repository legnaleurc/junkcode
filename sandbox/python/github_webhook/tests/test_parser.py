import unittest as ut

from triage_bot import parser as tbp


class TestParser(ut.TestCase):

    def testExtractURLs_SingleDomain(self):
        url = tbp.extractURLs('www.google.com')
        self.assertEqual(url, ['www.google.com'])

    def testExtractURLs_MultipleDomain(self):
        url = tbp.extractURLs('front www.google.com and www.mozilla.org end~~')
        self.assertEqual(url, [
            'www.google.com',
            'www.mozilla.org',
        ])
