# import asyncio
import concurrent.futures as cf
import sqlite3
import threading

from wcpan.worker import off_main_thread_method


off_main_thread = off_main_thread_method('_pool')


class AsyncConnection(object):

    def __init__(self, dsn):
        self._dsn = dsn
        self._tls = threading.local()
        self._pool = cf.ThreadPoolExecutor(max_workers=1,
                                           thread_name_prefix='AsyncConnection')

    @off_main_thread
    def cursor(self):
        db = self._get_thread_local_database()
        return AsyncCursor(db.cursor())

    @off_main_thread
    def commit(self):
        db = self._get_thread_local_database()
        db.commit()

    @off_main_thread
    def rollback(self):
        db = self._get_thread_local_database()
        db.rollback()

    @off_main_thread
    def close(self):
        db = self._get_thread_local_database()
        db.close()

    def _get_thread_local_database(self) -> sqlite3.Connection:
        db = getattr(self._tls, 'db', None)
        if db is None:
            db = self._open()
            setattr(self._tls, 'db', db)
        return db

    def _open(self) -> sqlite3.Connection:
        db = sqlite3.connect(self._dsn, detect_types=sqlite3.PARSE_DECLTYPES)
        db.row_factory = sqlite3.Row
        return db


class AsyncCursor(object):

    def __init__(self, cursor):
        self._query = cursor
