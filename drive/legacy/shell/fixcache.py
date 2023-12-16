#! /usr/bin/env python3

import asyncio
import contextlib
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.drive.crypt.util import decrypt_name
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'fixcache',
    ), '/tmp/fixcache.log')
    initialize_cache()

    root_path = sys.argv[1]

    factory = DriveFactory()
    factory.load_config()
    async with factory() as drive:
        node = await drive.get_node_by_path(root_path)
        async for root, folders, files in drive.walk(node):
            root_path = await drive.get_path(root)
            INFO('fixcache') << f'working on {root_path}'

            if is_fixed(root):
                INFO('fixcache') << f'(cached) no error in {root_path}'
                continue

            local_children = folders + files
            remote_children = await drive._remote._driver._fetch_children(root.id_)
            local_dict = {_.id_: _ for _ in local_children}
            remote_dict = {_.id_: _ for _ in remote_children}
            local_set = set((_.id_ for _ in local_children))
            remote_set = set((_.id_ for _ in remote_children))
            common_set = local_set.intersection(remote_set)
            hidden_set = remote_set.difference(common_set)
            orphan_set = local_set.difference(common_set)

            if not hidden_set and not orphan_set:
                INFO('fixcache') << f'no error in {root_path}'
                set_fixed(root)
                continue

            for orphan_id in orphan_set:
                orphan_node = local_dict[orphan_id]
                INFO('fixcache') << f'found orphan file {orphan_node.name}'
            assert not orphan_set

            for hidden_id in hidden_set:
                hidden_node = remote_dict[hidden_id]
                name = hidden_node.name
                if hidden_node.private and hidden_node.private.get('crypt', None) == '1':
                    name = decrypt_name(name)

                if hidden_node.trashed:
                    INFO('fixcache') << f'skip trashed item {name}'
                    continue

                await drive._remote._driver._force_update_by_id(hidden_id)
                INFO('fixcache') << f'force updated {name}'

    return 0


def initialize_cache():
    with sqlite3.connect('_fixed.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        try:
            query.execute('''
                CREATE TABLE fixed (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE
                );
            ''')
            query.execute('''
                CREATE INDEX ix_fixed_node_id ON fixed(node_id);
            ''')
        except sqlite3.OperationalError as e:
            print(e)
            pass


def is_fixed(node):
    with sqlite3.connect('_fixed.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        query.execute('''
            SELECT id FROM fixed WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def set_fixed(node):
    with sqlite3.connect('_fixed.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        query.execute('''
            INSERT INTO fixed (node_id) VALUES (?);
        ''', (node.id_,))


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
