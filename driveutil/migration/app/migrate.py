#! /usr/bin/env python3

import asyncio
import pathlib
import sqlite3
import sys

import arrow
from wcpan.drive.core.drive import Drive, Node
from wcpan.drive.core.types import MediaInfo
from wcpan.logger import setup as setup_logger, INFO

from common import (
    get_dst_drive,
    get_src_drive,
    humanize,
    is_migrated,
    migration_cache,
)


MAX_TASK_COUNT = 4
DAILY_SIZE = 500 * 1024 * 1024 * 1024
PATH_LIST = [
]


async def main(args: list[str] = None):
    setup_logger((
        'wcpan.drive',
        'migrate',
    ), './data/migrate.log')

    if args is None:
        args = sys.argv

    path_list = args[1:]
    path_list.extend(PATH_LIST)
    initialize_cache()

    src_factory = get_src_drive()
    dst_factory = get_dst_drive()

    async with src_factory() as src_drive,\
               dst_factory() as dst_drive:
        async for change in src_drive.sync():
            INFO('migrate') << 'src sync' << change
        async for change in dst_drive.sync():
            INFO('migrate') << 'dst sync' << change

        for path in path_list:
            await migrate(src_drive, dst_drive, path)


async def migrate(src_drive: Drive, dst_drive: Drive, from_path: str):
    src_root_node = await src_drive.get_node_by_path(from_path)
    assert src_root_node is not None, 'invalid source root'

    async for src_root, src_folders, src_files in src_drive.walk(src_root_node):
        migrated_size = get_migrated_size()
        INFO('migrate') << f'migrated {humanize(migrated_size)} today'
        if migrated_size >= DAILY_SIZE:
            break

        src_root_path = await src_drive.get_path(src_root)
        assert src_root_path is not None, 'invalid source path'

        dst_root = await get_node(dst_drive, src_root_path)
        assert dst_root is not None, 'invalid destination node'
        INFO('migrate') << f'working on {src_root_path}'

        quota = [DAILY_SIZE - migrated_size]
        lock = asyncio.Semaphore(MAX_TASK_COUNT)
        folder_list = [
            locked_migrate_folder(lock, src_drive, node, dst_drive, dst_root)
            for node in src_folders
        ]
        file_list = [
            locked_migrate_file(lock, src_drive, node, dst_drive, dst_root, quota)
            for node in src_files
        ]
        task_list = folder_list + file_list
        await asyncio.gather(*task_list)


async def migrate_folder(src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node):
    new_node = await dst_drive.get_node_by_name_from_parent(src_node.name, dst_root)
    if new_node:
        INFO('migrate') << 'folder exists, skip'
        return new_node

    new_node = await dst_drive.create_folder(dst_root, src_node.name, exist_ok=True)
    return new_node


async def migrate_file(src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node, quota: list[int]):
    new_node = await dst_drive.get_node_by_name_from_parent(src_node.name, dst_root)
    if new_node:
        if is_migrated(new_node):
            INFO('migrate') << 'file exists (cached), skip'
            return new_node
        if src_node.hash_ == new_node.hash_:
            set_migrated(new_node)
            INFO('migrate') << 'file exists (remote verified), skip'
            return new_node

    new_hash, new_node = await copy_node(src_drive, src_node, dst_drive, dst_root)
    assert new_node.size == src_node.size
    assert new_hash == new_node.hash_
    set_migrated(new_node)
    quota[0] -= new_node.size
    return new_node


async def locked_migrate_folder(lock: asyncio.Semaphore, src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node):
    async with lock:
        INFO('migrate') << f'mkdir {src_node.name}'
        await migrate_folder(src_drive, src_node, dst_drive, dst_root)
        INFO('migrate') << f'ok {src_node.name}'


async def locked_migrate_file(lock: asyncio.Semaphore, src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node, quota: list[int]):
    async with lock:
        if quota[0] <= 0:
            INFO('migrate') << 'skip (upload quota exceeded)'
            return
        INFO('migrate') << f'touch (@{humanize(quota[0])}) {src_node.name}'
        await migrate_file(src_drive, src_node, dst_drive, dst_root, quota)
        INFO('migrate') << f'ok (@{humanize(quota[0])}) {src_node.name}'


async def get_node(drive: Drive, path: pathlib.Path):
    while True:
        node = await drive.get_node_by_path(path)
        if node:
            return node

        await asyncio.sleep(1)
        async for change in drive.sync():
            INFO('migrate') << change


async def copy_node(src_drive: Drive, src_node: Node, dst_drive: Drive, new_root: Node):
    hasher = await dst_drive.get_hasher()
    media_info = None
    if src_node.is_image:
        media_info = MediaInfo.image(src_node.image_width, src_node.image_height)
    if src_node.is_video:
        media_info = MediaInfo.video(src_node.video_width, src_node.video_height, src_node.video_ms_duration)

    async with await dst_drive.upload(
        new_root,
        src_node.name,
        file_size=src_node.size,
        mime_type=src_node.mime_type,
        media_info=media_info,
    ) as fout:
        if src_node.size > 0:
            async with await src_drive.download(src_node) as fin:
                async for chunk in fin:
                    hasher.update(chunk)
                    await fout.write(chunk)

    new_hash = hasher.hexdigest()
    new_node = await fout.node()
    assert new_node is not None
    return new_hash, new_node


def initialize_cache():
    with migration_cache() as query:
        try:
            query.execute('''
                CREATE TABLE migrated (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE,
                    size INTEGER NOT NULL,
                    created_at INTEGER NOT NULL
                );
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_node_id ON migrated(node_id);
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_created_at ON migrated(created_at);
            ''')
        except sqlite3.OperationalError as e:
            print(e)
            pass


def set_migrated(node: Node):
    with migration_cache() as query:
        query.execute('''
            INSERT INTO migrated (node_id, size, created_at) VALUES (?, ?, ?);
        ''', (node.id_, node.size, node.created.timestamp))


def get_migrated_size() -> int:
    yesterday = arrow.utcnow().shift(days=-1).timestamp
    with migration_cache() as query:
        query.execute('SELECT SUM(size) FROM migrated WHERE created_at >= ?;', (yesterday,))
        row = query.fetchone()
        if not row:
            return 0
        if not row[0]:
            return 0
        return row[0]


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
