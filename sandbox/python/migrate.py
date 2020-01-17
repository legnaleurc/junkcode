#! /usr/bin/env python3

import asyncio
import contextlib
import pathlib
import re
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.drive.core.types import MediaInfo
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'migrate',
    ), '/tmp/migrate.log')

    from_path = sys.argv[1]
    initialize_cache()

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive:
        async for change in drive.sync():
            INFO('migrate') << change

        root_node = await drive.get_node_by_path(from_path)
        await migrate_root(drive, root_node)

        async for root, folders, files in drive.walk(root_node):
            root_path = await drive.get_path(root)
            new_root_path = pathlib.PurePath(*(('/', 'new') + root_path.parts[2:]))
            new_root = await get_node(drive, new_root_path)
            assert new_root is not None
            INFO('migrate') << f'working on {new_root_path}'

            lock = asyncio.Semaphore(6)
            folder_list = [locked_migrate_folder(lock, drive, node, new_root) for node in folders]
            file_list = [locked_migrate_file(lock, drive, node, new_root) for node in files]
            task_list = folder_list + file_list
            await asyncio.gather(*task_list)


async def migrate_root(drive, node):
    node_path = await drive.get_path(node)
    parts = node_path.parts
    assert parts[0] == '/'
    assert parts[1] == 'old'
    new_parts = ('/', 'new') + parts[2:]
    parent_node = await drive.get_root_node()
    for part in new_parts[1:]:
        node = await drive.get_node_by_name_from_parent(part, parent_node)
        if not node:
            node = await drive.create_folder(parent_node, part, exist_ok=True)
            while True:
                await asyncio.sleep(1)
                async for change in drive.sync():
                    INFO('migrate') << change
                node = await drive.get_node_by_name_from_parent(part, parent_node)
                if node:
                    break
        parent_node = node


async def migrate_folder(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        INFO('migrate') << 'folder exists, skip'
        return new_node

    new_node = await drive.create_folder(new_root, node.name, exist_ok=True)
    return new_node


async def migrate_file(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        if is_migrated(new_node):
            INFO('migrate') << 'file exists (cached), skip'
            return new_node
        new_hash = await get_node_hash(drive, node)
        if new_hash == new_node.hash_:
            set_migrated(new_node)
            INFO('migrate') << 'file exists (remote verified), skip'
            return new_node
    else:
        from wcpan.drive.crypt.util import encrypt_name
        remote_node = await drive._remote._driver._fetch_node_by_name_from_parent_id(encrypt_name(node.name), new_root.id_)
        if remote_node:
            INFO('migrate') << 'found wierd conflict, trash it'
            await drive.trash_node(remote_node)

    new_hash, new_node = await copy_node(drive, node, new_root)
    assert new_node.size == node.size
    assert new_hash == new_node.hash_
    set_migrated(new_node)
    return new_node


async def locked_migrate_folder(lock, drive, node, new_root):
    async with lock:
        INFO('migrate') << f'mkdir {node.name}'
        await migrate_folder(drive, node, new_root)
        INFO('migrate') << f'ok {node.name}'


async def locked_migrate_file(lock, drive, node, new_root):
    async with lock:
        INFO('migrate') << f'touch {node.name}'
        await migrate_file(drive, node, new_root)
        INFO('migrate') << f'ok {node.name}'


async def get_node(drive, path):
    while True:
        node = await drive.get_node_by_path(path)
        if node:
            return node

        await asyncio.sleep(1)
        async for change in drive.sync():
            INFO('migrate') << change


async def get_node_hash(drive, node):
    hasher = await drive.get_hasher()
    if node.size > 0:
        async with await drive.download(node) as fin:
            async for chunk in fin:
                hasher.update(chunk)
    rv = hasher.hexdigest()
    return rv


async def copy_node(drive, node, new_root):
    hasher = await drive.get_hasher()
    media_info = None
    if node.is_image:
        media_info = MediaInfo.image(node.image_width, node.image_height)
    if node.is_video:
        media_info = MediaInfo.video(node.video_width, node.video_height, node.video_ms_duration)

    async with await drive.upload(
        new_root,
        node.name,
        file_size=node.size,
        mime_type=node.mime_type,
        media_info=media_info,
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
