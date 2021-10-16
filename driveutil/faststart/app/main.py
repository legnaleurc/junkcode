#! /usr/bin/env python3

import asyncio
import sys
import tempfile

from wcpan.drive.core.drive import DriveFactory, Drive, Node

from .processor import create_processor


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    base_url = args[1]
    root_path = args[2]

    factory = DriveFactory()
    factory.load_config()

    with tempfile.TemporaryDirectory() as work_folder:
        async with factory() as drive:
            root_node = await drive.get_node_by_path(root_path)
            assert root_node is not None
            async for root, folders, files in drive.walk(root_node):
                for file_ in files:
                    if not file_.is_video:
                        continue

                    processor = create_processor(base_url, work_folder, drive, file_)
                    await processor()

    return 0
