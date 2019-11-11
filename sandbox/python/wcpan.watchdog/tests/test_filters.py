import unittest as ut

import wcpan.watchdog.filters as wwdf

from .util import create_dir_entry, create_file_entry


class TestFilters(ut.TestCase):

    def testDefault(self):
        filter_ = wwdf.create_default_filter()

        entry = create_dir_entry('.git')
        self.assertFalse(filter_(entry))

        entry = create_dir_entry('node_modules')
        self.assertFalse(filter_(entry))

        entry = create_dir_entry('test')
        self.assertTrue(filter_(entry))
        entry = create_file_entry('test')
        self.assertTrue(filter_(entry))
