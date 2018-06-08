import asyncio
import functools as ft


SQL_CREATE_TABLE = [
    '''
    CREATE TABLE people (
        id INTEGER PRIMARY KEY,
        name TEXT
    );
    ''',
]


class ReadOnly(object):

    def __init__(self, db):
        self._db = db

    async def __aenter__(self):
        self._cursor = await self._db.cursor()
        return self._cursor

    async def __aexit__(self, exc_type, exc_value, exc_tb):
        await self._cursor.close()


class ReadWrite(object):

    def __init__(self, db):
        self._db = db

    async def __aenter__(self):
        self._cursor = await self._db.cursor()
        return self._cursor

    async def __aexit__(self, exc_type, exc_value, exc_tb):
        if exc_type is None:
            await self._db.commit()
        else:
            await self._db.rollback()
        await self._cursor.close()


def sync(method):
    @ft.wraps(method)
    def wrapper(self, *args, **kwargs):
        loop = asyncio.get_event_loop()
        f = method(self, *args, **kwargs)
        return loop.run_until_complete(f)
    return wrapper
