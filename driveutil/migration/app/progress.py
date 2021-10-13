#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import Drive, Node

from common import get_dst_drive, get_src_drive, is_migrated


async def main():
    from_path = sys.argv[1]
    src_factory = get_src_drive()
    dst_factory = get_dst_drive()
    async with src_factory() as src_drive, \
               dst_factory() as dst_drive:
        root_node = await src_drive.get_node_by_path(from_path)
        assert root_node is not None

        children = await src_drive.get_children(root_node)
        task_list = [
            calculate_progress(src_drive, dst_drive, folder)
            for folder in children
        ]
        result_list = await asyncio.gather(*task_list)

        for folder, rv in result_list:
            print(f'{folder.name}: {rv * 100:.2f}%')


async def calculate_progress(src_drive: Drive, dst_drive: Drive, root_node: Node):
    total: int = 0
    migrated: int = 0
    async for root, folders, files in src_drive.walk(root_node):
        for file_ in files:
            total += file_.size
            path = await src_drive.get_path(file_)
            dst = await dst_drive.get_node_by_path(path)
            if dst and is_migrated(dst):
                migrated += file_.size
    return root_node, migrated / total


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
