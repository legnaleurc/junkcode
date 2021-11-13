#! /usr/bin/env python3

import contextlib
import pathlib
import sqlite3

import arrow
from wcpan.drive.core.util import get_default_data_path


def main():
    data_path = get_default_data_path()
    cache_path = data_path / 'nodes.sqlite'

    with cache_selector(cache_path) as query:
        now = arrow.utcnow()
        yesterday = now.shift(days=-1)
        past = yesterday.timestamp
        total = 0
        for h in range(24):
            size = get_range(query, past + h * 3600)
            print(humanize(size))
            total += size
    print(f'Total: {humanize(total)}')


@contextlib.contextmanager
def cache_selector(cache_path: pathlib.Path) -> sqlite3.Cursor:
    with sqlite3.connect(cache_path) as db, \
         contextlib.closing(db.cursor()) as query:
        yield query


def get_range(query: sqlite3.Cursor, timestamp):
    query.execute('''
        SELECT SUM(size)
        FROM files
        LEFT JOIN nodes
        ON files.id == nodes.id
        WHERE created >= ? AND created < ?;
    ''', (timestamp, timestamp + 3600))
    rv = query.fetchone()
    if rv is None:
        return 0
    if rv[0] is None:
        return 0
    return rv[0]


def humanize(n: int) -> str:
    unit_list = ['', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB']
    e = 0
    while n >= 1024:
        n = n // 1024
        e += 1
    return f'{n}{unit_list[e]}'


if __name__ == '__main__':
    main()
