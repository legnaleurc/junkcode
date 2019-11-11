import unittest as ut

import wcpan.watchdog.filters as wwdf

from .util import create_dir_entry, create_file_entry


class TestFilters(ut.TestCase):

    def testDefault(self):
        filter_ = wwdf.create_default_filter()

        with create_dir_entry('.git') as entry:
            self.assertFalse(filter_(entry))

        with create_dir_entry('node_modules') as entry:
            self.assertFalse(filter_(entry))

        with create_dir_entry('test') as entry:
            self.assertTrue(filter_(entry))
        with create_file_entry('test') as entry:
            self.assertTrue(filter_(entry))

    def testGlob1(self):
        filter_ = wwdf.matches_glob('*')

        with create_file_entry('a') as entry:
            self.assertTrue(filter_(entry))

        with create_dir_entry('a') as entry:
            self.assertTrue(filter_(entry))

        with create_file_entry('a/b') as entry:
            self.assertTrue(filter_(entry))

    def testGlob2(self):
        filter_ = wwdf.matches_glob('a/*')

        with create_file_entry('a') as entry:
            self.assertFalse(filter_(entry))

        with create_dir_entry('a') as entry:
            self.assertTrue(filter_(entry))

        with create_file_entry('a/b') as entry:
            self.assertTrue(filter_(entry))

        with create_file_entry('a/b/c') as entry:
            self.assertTrue(filter_(entry))
