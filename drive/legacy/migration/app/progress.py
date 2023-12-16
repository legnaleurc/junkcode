#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import Drive, Node

from common import get_dst_drive, get_src_drive, humanize


async def main():
    from_path = sys.argv[1]
    src_factory = get_src_drive()
    dst_factory = get_dst_drive()
    async with src_factory() as src_drive, \
               dst_factory() as dst_drive:
        root_node = await src_drive.get_node_by_path(from_path)
        assert root_node is not None

        children = await src_drive.get_children(root_node)
        for src_node in children:
            if src_node.is_file:
                continue

            path = await src_drive.get_path(src_node)
            if not path:
                continue
            dst_node = await dst_drive.get_node_by_path(path)
            if not dst_node:
                print(f'{src_node.name}: 0.00%')
                continue

            src_total = await calculate_total_size(src_drive, src_node)
            dst_total = await calculate_total_size(dst_drive, dst_node)
            diff = src_total - dst_total
            print(f'{dst_node.name}: {(dst_total / src_total) * 100:.2f}% ({humanize(diff)})')


async def calculate_total_size(drive: Drive, root_node: Node):
    assert root_node.is_folder
    total: int = 0
    async for root, folders, files in drive.walk(root_node):
        for file_ in files:
            total += file_.size
    return total


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
