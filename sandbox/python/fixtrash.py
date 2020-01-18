#! /usr/bin/env python3

import asyncio
import contextlib
import pathlib
import re
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.drive.core.types import MediaInfo
from wcpan.drive.crypt.util import encrypt_name
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'fixtrash',
    ), '/tmp/fixtrash.log')

    from_path = sys.argv[1]

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive:
        async for change in drive.sync():
            INFO('fixtrash') << change

        root_node = await drive.get_node_by_path(from_path)
        await migrate_root(drive, root_node)

        async for root, folders, files in drive.walk(root_node):
            root_path = await drive.get_path(root)
            new_root_path = pathlib.PurePath(*(('/', 'new') + root_path.parts[2:]))
            new_root = await get_node(drive, new_root_path)
            if not new_root:
                INFO('fixtrash') << f'not yet completed {new_root_path}, skip'
                continue

            INFO('fixtrash') << f'working on {new_root_path}'

            if not await drive._remote._driver._has_remote_children(new_root):
                INFO('fixtrash') << f'no child, skip'
                continue

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
                    INFO('fixtrash') << change
                node = await drive.get_node_by_name_from_parent(part, parent_node)
                if node:
                    break
        parent_node = node


async def migrate_folder(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        INFO('fixtrash') << 'folder exists, skip'
    else:
        name = encrypt_name(node.name)
        remote_node = await drive._remote._driver._fetch_node_by_name_from_parent_id(name, new_root.id_)
        if remote_node:
            INFO('fixtrash') << 'found wierd conflict, try force update'
            await drive._remote._driver._force_update_node(name, new_root)


async def migrate_file(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        INFO('fixtrash') << 'file exists, skip'
    else:
        name = encrypt_name(node.name)
        remote_node = await drive._remote._driver._fetch_node_by_name_from_parent_id(name, new_root.id_)
        if remote_node:
            INFO('fixtrash') << 'found wierd conflict, try force update'
            await drive._remote._driver._force_update_node(name, new_root)


async def locked_migrate_folder(lock, drive, node, new_root):
    async with lock:
        INFO('fixtrash') << f'mkdir {node.name}'
        await migrate_folder(drive, node, new_root)
        INFO('fixtrash') << f'ok {node.name}'


async def locked_migrate_file(lock, drive, node, new_root):
    async with lock:
        INFO('fixtrash') << f'touch {node.name}'
        await migrate_file(drive, node, new_root)
        INFO('fixtrash') << f'ok {node.name}'


async def get_node(drive, path):
    node = await drive.get_node_by_path(path)
    return node


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


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
