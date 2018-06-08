import unittest as ut

import wcpan.database as wd

from .util import sync


class TestSQLite(ut.TestCase):

    @sync
    async def setUp(self):
        self._db = await wd.connect('sqlite:///:memory:')

    @sync
    async def tearDown(self):
        await self._db.close()

    @sync
    async def testExecute(self):
        pass
