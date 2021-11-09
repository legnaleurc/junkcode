#! /usr/bin/env python3

import asyncio
import contextlib
import pathlib
import shutil
import sys
import tempfile

from wcpan.drive.core.drive import DriveFactory, Drive
from wcpan.drive.core.types import Node
from wcpan.drive.core.util import download_to_local
from wcpan.drive.cli.util import get_image_info, get_video_info
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'fixmedia',
    ))

    from_path = sys.argv[1]

    factory = DriveFactory()

    with tempfile.TemporaryDirectory as work:
        work_folder = pathlib.Path(work)

        async with factory() as drive:
            async for change in drive.sync():
                INFO('fixmedia') << change

            root_node = await drive.get_node_by_path(from_path)

            async for root, dummy_folders, files in drive.walk(root_node):
                path = await drive.get_path(root)
                INFO('fixmedia') << f'working on {path}'

                lock = asyncio.Semaphore(4)
                task_list = [locked_fix_file(lock, work_folder, drive, node) for node in files]
                await asyncio.gather(*task_list)


async def locked_fix_file(lock: asyncio.Semaphore, work_folder: pathlib.Path, drive: Drive, node: Node):
    async with lock:
        with local_context(work_folder, node) as local_folder:
            await fix_file(local_folder, drive, node)


async def fix_file(local_folder: pathlib.Path, drive: Drive, node: Node):
    if node.mime_type.startswith('image/') and not node.is_image:
        INFO('fixmedia') << f'fixing {node.name}'
        local_file = await download_to_local(drive, node, local_folder)
        media_info = get_image_info(local_file)
        await drive._remote._driver._client.files.update(node.id_, media_info=media_info)
        INFO('fixmedia') << f'fixed to {media_info.width} {media_info.height}'

    if node.mime_type.startswith('video/') and not node.is_video:
        INFO('fixmedia') << f'fixing {node.name}'
        local_file = await download_to_local(drive, node, local_folder)
        media_info = await get_video_info(local_file)
        await drive._remote._driver._client.files.update(node.id_, media_info=media_info)
        INFO('fixmedia') << f'fixed to {media_info.width} {media_info.height} {media_info.ms_duration}'


@contextlib.contextmanager
def local_context(work_folder: pathlib.Path, node: Node):
    local_folder = work_folder / node.id_
    local_folder.mkdir(exist_ok=True)
    try:
        yield local_folder
    finally:
        shutil.rmtree(local_folder)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
