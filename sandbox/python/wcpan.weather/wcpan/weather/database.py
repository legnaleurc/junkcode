import json
import sqlite3
from typing import Any, Dict, List, Text, Union


SQL_CREATE_TABLES = [
    '''
    CREATE TABLE country (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT
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

    def update_city(self, city_data):
        country_id = self.get_country_id_by_name(city_data['country'])
        if not country_id:
            country_id = self.create_country(city_data['country'])

        with ReadWrite(self._db) as query:
            query.execute('''
                INSERT INTO city
                (id, name, country, lon, lat)
                VALUES
                (?, ?, ?, ?, ?)
            ;''', (city_data['id'], city_data['name'], country_id,
            city_data['coord']['lon'], city_data['coord']['lat']))

    def get_country_id_by_name(self, name):
        with ReadOnly(self._db) as query:
            query.execute('SELECT id FROM country WHERE name=?;', (name,))
            rv = query.fetchone()
        if not rv:
            return None
        return rv['id']

    def create_country(self, name):
        with ReadWrite(self._db) as query:
            query.execute('INSERT INTO country (name) VALUES (?);', (name,))
            return query.lastrowid

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


def initialize(city_list_file, dsn):
    with open(city_list_file, 'r') as fin, \
         Database(dsn) as db:
        cities = json.load(fin)
        for city in cities:
            db.update_city(city)


if __name__ == '__main__':
    import sys
    initialize(sys.argv[1], sys.argv[2])
