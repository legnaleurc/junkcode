#! /usr/bin/env python3

import asyncio
import contextlib
import pathlib
import sqlite3
import sys

import arrow
from wcpan.drive.core.drive import DriveFactory, Drive, Node
from wcpan.drive.core.types import MediaInfo
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'migrate',
    ), '/tmp/migrate.log')

    from_path = sys.argv[1]
    initialize_cache()

    src_factory = get_src_drive()
    dst_factory = get_dst_drive()

    async with src_factory() as src_drive,\
               dst_factory() as dst_drive:
        async for change in src_drive.sync():
            INFO('migrate') << 'src sync' << change
        async for change in dst_drive.sync():
            INFO('migrate') << 'dst sync' << change

        src_root_node = await src_drive.get_node_by_path(from_path)
        assert src_root_node is not None, 'invalid source root'

        async for src_root, src_folders, src_files in src_drive.walk(src_root_node):
            migrated_size = get_migrated_size()
            INFO('migrate') << f'migrated {migrated_size} today'
            if migrated_size >= 700 * 1024 * 1024 * 1024:
                break

            src_root_path = await src_drive.get_path(src_root)
            assert src_root_path is not None, 'invalid source path'
            if str(src_root_path).startswith('/tmp'):
                INFO('migrate') << f'skip /tmp'
                continue
            dst_root = await get_node(dst_drive, src_root_path)
            assert dst_root is not None, 'invalid destination node'
            INFO('migrate') << f'working on {src_root_path}'

            lock = asyncio.Semaphore(4)
            folder_list = [
                locked_migrate_folder(lock, src_drive, node, dst_drive, dst_root)
                for node in src_folders
            ]
            file_list = [
                locked_migrate_file(lock, src_drive, node, dst_drive, dst_root)
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


async def migrate_file(src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node):
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
    return new_node


async def locked_migrate_folder(lock: asyncio.Semaphore, src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node):
    async with lock:
        INFO('migrate') << f'mkdir {src_node.name}'
        await migrate_folder(src_drive, src_node, dst_drive, dst_root)
        INFO('migrate') << f'ok {src_node.name}'


async def locked_migrate_file(lock: asyncio.Semaphore, src_drive: Drive, src_node: Node, dst_drive: Drive, dst_root: Node):
    async with lock:
        INFO('migrate') << f'touch {src_node.name}'
        await migrate_file(src_drive, src_node, dst_drive, dst_root)
        INFO('migrate') << f'ok {src_node.name}'


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
    return new_hash, new_node


@contextlib.contextmanager
def migration_cache():
    with sqlite3.connect('_migrated.sqlite') as db, \
        contextlib.closing(db.cursor()) as query:
        yield query


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


def is_migrated(node: Node):
    with migration_cache() as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


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


def get_config_path(root: str):
    path = pathlib.Path(f'./{root}/config')
    path = path.expanduser()
    path = path / 'wcpan.drive'
    return path


def get_data_path(root: str):
    path = pathlib.Path(f'./{root}/local/share')
    path = path.expanduser()
    path = path / 'wcpan.drive'
    return path


def get_src_drive():
    factory = DriveFactory()
    factory.data_path = get_data_path('src')
    factory.load_config()
    return factory


def get_dst_drive():
    factory = DriveFactory()
    factory.data_path = get_data_path('dst')
    factory.load_config()
    return factory


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
