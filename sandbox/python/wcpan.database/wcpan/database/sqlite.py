import asyncio
import concurrent.futures as cf
import functools as ft
import sqlite3
import threading


def background(method):
    @ft.wraps(method)
    def wrapper(self, *args, **kwargs):
        bound_fn = ft.partial(method, self, *args, **kwargs)
        return self._bg.call_sync(bound_fn)
    return wrapper


class AsyncConnection(object):

    def __init__(self, dsn):
        self._dsn = dsn
        self._bg = BackgroundEventLoop()
        self._db = None

    async def __aenter__(self):
        await self._bg.__aenter__()
        return self

    async def __aexit__(self, exc_type, exc, exc_tb):
        await self.close()
        await self._bg.__aexit__(exc_type, exc, exc_tb)

    @background
    def cursor(self):
        db = self._get_db()
        return AsyncCursor(self._bg, db.cursor())

    @background
    def commit(self):
        db = self._get_db()
        db.commit()

    @background
    def rollback(self):
        db = self._get_db()
        db.rollback()

    @background
    def close(self):
        db = self._get_db()
        db.close()

    def _get_db(self):
        assert not is_main_thread()
        if not self._db:
            self._db = self._open()
        return self._db

    def _open(self):
        assert not is_main_thread()
        db = sqlite3.connect(self._dsn, detect_types=sqlite3.PARSE_DECLTYPES)
        db.row_factory = sqlite3.Row
        return db


class AsyncCursor(object):

    def __init__(self, bg, cursor):
        self._bg = bg
        self._query = cursor

    @background
    def close(self):
        self._query.close()

    @background
    def execute(self, operation, parameters=None):
        if parameters is None:
            return self._query.execute(operation)
        else:
            return self._query.execute(operation, parameters)

    @background
    def executemany(self, operation, seq_of_parameters):
        return self._query.executemany(operation, seq_of_parameters)

    @background
    def fetchone(self):
        return self._query.fetchone()

    @background
    def fetchmany(self, size=None):
        if size is None:
            return self._query.fetchmany()
        else:
            return self._query.fetchmany(size)

    @background
    def fetchall(self):
        return self._query.fetchall()

    @property
    def arraysize(self):
        return self._query.arraysize

    @arraysize.setter
    def arraysize(self, v):
        self._query.arraysize = v


class BackgroundEventLoop(object):

    def __init__(self):
        assert is_main_thread()
        self._thread = threading.Thread(target=self._run)
        self._loop = asyncio.new_event_loop()

    async def __aenter__(self):
        assert is_main_thread()
        self._thread.start()
        return self

    async def __aexit__(self, exc_type, exc, exc_tb):
        assert is_main_thread()
        self._loop.call_soon_threadsafe(self._loop.stop)
        self._thread.join()

    def _run(self):
        assert not is_main_thread()
        self._loop.run_forever()
        self._loop.close()

    def call_sync(self, fn):
        assert is_main_thread()
        main_loop = asyncio.get_event_loop()
        future = main_loop.create_future()
        cb = ft.partial(self._proxy, fn, future, main_loop)
        self._loop.call_soon_threadsafe(cb)
        return future

    def _proxy(self, fn, future, main_loop):
        assert not is_main_thread()
        try:
            rv = fn()
            cb = ft.partial(future.set_result, rv)
        except Exception as e:
            cb = ft.partial(future.set_exception, e)
        main_loop.call_soon_threadsafe(cb)


def is_main_thread():
    return threading.current_thread() == threading.main_thread()
