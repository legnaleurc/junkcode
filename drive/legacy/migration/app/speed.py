#! /usr/bin/env python3

import arrow

from common import humanize, migration_cache


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
    with migration_cache() as query:
        query.execute('SELECT SUM(size) FROM migrated WHERE created_at >= ? AND created_at < ?;', (timestamp, timestamp + 3600))
        rv = query.fetchone()
        if rv is None:
            return 0
        if rv[0] is None:
            return 0
        return rv[0]


if __name__ == '__main__':
    main()
