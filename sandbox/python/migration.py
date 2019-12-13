#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import DriveFactory


FROM_FOLDER = '/old/Documents'


async def main():
    factory = DriveFactory()
    factory.set_database('nodes.sqlite')
    factory.set_driver('wcpan.drive.google')
    factory.add_middleware('wcpan.drive.crypt')

    async with factory.create_drive() as drive:
        async for change in drive.sync():
            print(change)

        root_node = await drive.get_node_by_path(FROM_FOLDER)
        async for root, folders, files in drive.walk(root_node):
            print(root, folders, files)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
