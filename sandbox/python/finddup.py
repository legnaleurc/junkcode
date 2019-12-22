#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'finddup',
    ))

    from_path = sys.argv[1]

    factory = DriveFactory()
    factory.database = 'nodes.sqlite'
    factory.driver = 'wcpan.drive.google'
    factory.middleware_list.append('wcpan.drive.crypt')

    async with factory() as drive:
        async for change in drive.sync():
            INFO('finddup') << change

        root_node = await drive.get_node_by_path(from_path)
        dup_list = []
        async for dummy_root, folders, files in drive.walk(root_node):
            nodes = folders + files
            seen = {}
            for node in nodes:
                if node.name not in seen:
                    seen[node.name] = [node.id_]
                else:
                    seen[node.name].append(node.id_)
            for id_list in seen.values():
                if len(id_list) > 1:
                    dup_list.append(id_list)

    for id_list in dup_list:
        print(id_list)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))

