#! /usr/bin/env python3

import contextlib
import sqlite3

import arrow


def main():
    now = arrow.utcnow()
    yesterday = now.shift(days=-1)
    past = yesterday.timestamp
    total = 0
    for h in range(24):
        size = get_range(past + h * 3600)
        print(humanize(size))
        total += size
    print(f'Total: {humanize(total)}')


def get_range(timestamp):
    with sqlite3.connect('_migrated.sqlite') as db, \
         contextlib.closing(db.cursor()) as query:
        query.execute('SELECT SUM(size) FROM migrated WHERE created_at >= ? AND created_at < ?;', (timestamp, timestamp + 3600))
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
