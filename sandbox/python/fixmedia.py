#! /usr/bin/env python3

import asyncio
import contextlib
import mimetypes
import pathlib
import re
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'fixmedia',
    ))

    from_path = sys.argv[1]

    factory = DriveFactory()
    factory.database = 'nodes.sqlite'
    factory.driver = 'wcpan.drive.google'
    factory.middleware_list.append('wcpan.drive.crypt')

    async with factory() as drive:
        async for change in drive.sync():
            INFO('fixmedia') << change

        root_node = await drive.get_node_by_path(from_path)

        async for root, dummy_folders, files in drive.walk(root_node):
            root_path = await drive.get_path(root)
            new_root_path = pathlib.PurePath(*(('/', 'new') + root_path.parts[2:]))
            new_root = await drive.get_node_by_path(new_root_path)
            if not new_root:
                INFO('fixmedia') << f'{new_root_path} not exist, skip'
                continue

            INFO('fixmedia') << f'working on {new_root_path}'

            lock = asyncio.Semaphore(5)
            task_list = [locked_fix_file(lock, drive, node, new_root) for node in files]
            await asyncio.gather(*task_list)


async def locked_fix_file(lock, drive, node, new_root):
    async with lock:
        INFO('fixmedia') << f'touch {node.name}'
        await fix_file(drive, node, new_root)
        INFO('fixmedia') << f'ok {node.name}'


async def fix_file(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if not new_node:
        INFO('fixmedia') << 'file missing'
        return

    assert new_node.size == node.size

    if node.is_image:
        if not new_node.is_image or new_node.image_width <= 0 or new_node.image_height <= 0:
            await drive._remote._set_node_image_metadata(new_node, width=node.image_width, height=node.image_height)
            INFO('fixmedia') << f'fixed to {node.image_width} {node.image_height}'
        else:
            INFO('fixmedia') << f'({node.image_width}, {node.image_height}) == ({new_node.image_width}, {new_node.image_height})'
    if node.is_video:
        if not new_node.is_video or new_node.video_width <= 0 or new_node.video_height <= 0 or node.video_ms_duration <= 0:
            await drive._remote._set_node_video_metadata(new_node, width=node.video_width, height=node.video_height, ms_duration=node.video_ms_duration)
            INFO('fixmedia') << f'fixed to {node.video_width} {node.video_height} {node.video_ms_duration}'
        else:
            INFO('fixmedia') << f'({node.video_width}, {node.video_height}, {node.video_ms_duration}) == ({new_node.video_width}, {new_node.video_height}, {new_node.video_ms_duration})'

    return new_node


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
