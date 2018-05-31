import sqlite3
from typing import Any, Dict, List, Text, Union


SQL_CREATE_TABLES = [
    '''
    CREATE TABLE country (
        id INTEGER NOT NULL,
        name TEXT,
        PRIMARY KEY (id),
    );
    ''',
    '''
    CREATE TABLE city (
        id INTEGER NOT NULL,
        name TEXT,
        country INTEGER,
        lon REAL,
        lat REAL,
        PRIMARY KEY (id),
        FOREIGN KEY (country) REFERENCES country (id)
    );
    ''',
    'CREATE INDEX ix_city_country ON city(country);',
]


class Database(object):

    def __init__(self, dsn) -> None:
        self._dsn = dsn

    def __enter__(self) -> 'Database':
        try:
            self._try_create()
        except sqlite3.OperationalError as e:
            pass
        return self

    def __exit__(self, exc_type, exc, tb) -> bool:
        self._db.close()
        self._db = None

    def _open(self) -> sqlite3.Connection:
        db = sqlite3.connect(self._dsn)
        db.row_factory = sqlite3.Row
        return db

    def _try_create(self) -> None:
        self._db = self._open()
        with ReadWrite(self._db) as query:
            for sql in SQL_CREATE_TABLES:
                query.execute(sql)


class ReadOnly(object):

    def __init__(self, db: sqlite3.Connection) -> None:
        self._db = db

    def __enter__(self) -> sqlite3.Cursor:
        self._cursor = self._db.cursor()
        return self._cursor

    def __exit__(self, exc_type, exc_value, exc_tb) -> bool:
        self._cursor.close()


class ReadWrite(object):

    def __init__(self, db: sqlite3.Connection) -> None:
        self._db = db

    def __enter__(self) -> sqlite3.Cursor:
        self._cursor = self._db.cursor()
        return self._cursor

    def __exit__(self, exc_type, exc_value, exc_tb) -> bool:
        if exc_type is None:
            self._db.commit()
        else:
            self._db.rollback()
        self._cursor.close()
