import unittest as ut

import wcpan.database as wd

from . import util as u


class TestSQLite(ut.TestCase):

    @u.sync
    async def setUp(self):
        self._db = await wd.connect('sqlite:///:memory:')

        async with u.ReadWrite(self._db) as query:
            for sql in u.SQL_CREATE_TABLE:
                await query.execute(sql)

    @u.sync
    async def tearDown(self):
        await self._db.close()

    @u.sync
    async def testExecute(self):
        pass
