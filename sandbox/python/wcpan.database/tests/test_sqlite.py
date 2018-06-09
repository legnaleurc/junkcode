import unittest as ut

from async_exit_stack import AsyncExitStack
import wcpan.database as wd

from . import util as u


class TestSQLite(ut.TestCase):

    @u.sync
    async def setUp(self):
        async with AsyncExitStack() as stack:
            self._db = await stack.enter_async_context(wd.connect('sqlite:///:memory:'))
            self._stack = stack

        async with u.ReadWrite(self._db) as query:
            for sql in u.SQL_CREATE_TABLE:
                await query.execute(sql)

    @u.sync
    async def tearDown(self):
        await self._stack.aclose()

    @u.sync
    async def testExecute(self):
        pass
