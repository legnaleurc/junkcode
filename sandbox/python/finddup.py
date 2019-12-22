#! /usr/bin/env python3

import asyncio
import sys

import yaml

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
                    seen[node.name] = [node.to_dict()]
                else:
                    seen[node.name].append(node.to_dict())
            for node_list in seen.values():
                if len(node_list) > 1:
                    dup_list.append(node_list)

    yaml.safe_dump(dup_list, default_flow_style=False)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))

