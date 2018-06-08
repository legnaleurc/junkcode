import unittest as ut

import wcpan.database as wd

from .util import await_


class TestSQLite(ut.TestCase):

    def setUp(self):
        self._db = await_(wd.connect('sqlite:///:memory:'))

    def tearDown(self):
        await_(self._db.close())
