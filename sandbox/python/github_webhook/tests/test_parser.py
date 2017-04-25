import unittest as ut

from triage_bot import parser as tbp


class TestParser(ut.TestCase):

    def testExtractURLs(self):
        url = tbp.extractURLs('www.google.com')
        self.assertEqual(url, ['www.google.com'])
