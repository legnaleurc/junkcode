#! /usr/bin/env python3

import asyncio
import contextlib
import mimetypes
import pathlib
import re
import sqlite3
import sys

from wcpan.drive.core.drive import DriveFactory


async def main():
    factory = DriveFactory()
    factory.database = 'nodes.sqlite'
    factory.driver = 'wcpan.drive.google'
    factory.middleware_list.append('wcpan.drive.crypt')

    async with factory() as drive:
        async for change in drive.sync():
            print(change)

        total_size = 0
        root_node = await drive.get_node_by_path(sys.argv[1])
        async for root, folders, files in drive.walk(root_node):
            sub_size = (node.size for node in files)
            total_size += sum(sub_size)

        print(f'{total_size:,}')

    return 0


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
