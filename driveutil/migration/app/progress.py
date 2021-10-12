#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import Drive, Node

from common import get_src_drive, is_migrated


async def main():
    from_path = sys.argv[1]
    factory = get_src_drive()
    async with factory() as drive:
        root_node = await drive.get_node_by_path(from_path)
        assert root_node is not None

        children = await drive.get_children(root_node)
        task_list = [
            (folder, calculate_progress(drive, folder))
            for folder in children
        ]
        result_list = await asyncio.gather(*task_list)

        for folder, rv in result_list:
            print(f'{folder.name}: {rv * 100:.2f}%')


async def calculate_progress(drive: Drive, root_node: Node):
    total: int = 0
    migrated: int = 0
    async for root, folders, files in drive.walk(root_node):
        for file in files:
            total += file.size
            if is_migrated(file):
                migrated += file.size
    return migrated / total


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
