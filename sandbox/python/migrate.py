#! /usr/bin/env python3

import asyncio
import contextlib
import mimetypes
import pathlib
import re
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory


FROM_FOLDER = '/old/Pictures'
TO_FOLDER = '/new'


async def main():
    initialize_cache()

    factory = DriveFactory()
    factory.database = 'nodes.sqlite'
    factory.driver = 'wcpan.drive.google'
    factory.middleware_list.append('wcpan.drive.crypt')

    async with factory() as drive:
        async for change in drive.sync():
            print(change)

        root_node = await drive.get_node_by_path(FROM_FOLDER)
        new_root_node = await drive.get_node_by_path(TO_FOLDER)
        await migrate_root(drive, root_node, new_root_node)

        async for root, folders, files in drive.walk(root_node):
            root_path = await drive.get_path(root)
            new_root_path = re.sub(r'^/old', '/new', str(root_path))
            new_root = await drive.get_node_by_path(new_root_path)
            assert new_root is not None
            print(f'working on {new_root_path}')

            for node in folders:
                print(f'mkdir {node.name}')
                await migrate_folder(drive, node, new_root)
                print('ok')

            for node in files:
                print(f'touch {node.name}')
                await migrate_file(drive, node, new_root)
                print('ok')


async def migrate_root(drive, node, new_root):
    node_path = await drive.get_path(node)
    parts = node_path.parts
    new_parts = parts[0:1] + ('new',) + parts[2:]
    parent_node = await drive.get_root_node()
    for part in new_parts[1:]:
        node = await drive.get_node_by_name_from_parent(part, parent_node)
        if not node:
            node = await drive.create_folder(parent_node, part, exist_ok=True)
            while True:
                await asyncio.sleep(1)
                async for change in drive.sync():
                    print(change)
                node = await drive.get_node_by_name_from_parent(part, parent_node)
                if node:
                    break
        parent_node = node


async def migrate_folder(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        print('folder exists, skip')
        return new_node

    new_node = await drive.create_folder(new_root, node.name, exist_ok=True)
    while True:
        await asyncio.sleep(1)
        async for change in drive.sync():
            print(change)
        new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
        if new_node:
            return new_node


async def migrate_file(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        if is_migrated(new_node):
            print('file exists (cached), skip')
            return new_node
        new_hash = await get_node_hash(drive, node)
        if new_hash == new_node.hash_:
            set_migrated(new_node)
            print('file exists (remote verified), skip')
            return new_node

    new_hash, new_node = await copy_node(drive, node, new_root)
    assert new_hash == new_node.hash_
    set_migrated(new_node)
    return new_node


async def get_node_hash(drive, node):
    hasher = await drive.get_hasher()
    if node.size > 0:
        async with await drive.download(node) as fin:
            async for chunk in fin:
                hasher.update(chunk)
    rv = hasher.hexdigest()
    return rv


async def copy_node(drive, node, new_root):
    mt, _ = mimetypes.guess_type(node.name)
    hasher = await drive.get_hasher()

    async with await drive.upload(
        new_root,
        node.name,
        node.size,
        mt,
    ) as fout:
        if node.size > 0:
            async with await drive.download(node) as fin:
                async for chunk in fin:
                    hasher.update(chunk)
                    await fout.write(chunk)

    new_hash = hasher.hexdigest()
    new_node = await fout.node()
    return new_hash, new_node


def initialize_cache():
    with sqlite3.connect('_migrated.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        try:
            query.execute('''
                CREATE TABLE migrated (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE
                );
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_node_id ON migrated(node_id);
            ''')
        except sqlite3.OperationalError as e:
            print(e)
            pass


def is_migrated(node):
    with sqlite3.connect('_migrated.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def set_migrated(node):
    with sqlite3.connect('_migrated.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        query.execute('''
            INSERT INTO migrated (node_id) VALUES (?);
        ''', (node.id_,))


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
