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
            await purge_folder(dup_list, drive, folders)
            await purge_file(dup_list, drive, files)

    with open('/tmp/x.yaml', 'wb') as fout:
        yaml.safe_dump(dup_list, stream=fout, default_flow_style=False, encoding='utf-8', allow_unicode=True)


async def purge_folder(rv, drive, folder_list):
    seen = {}
    for node in folder_list:
        if node.name not in seen:
            seen[node.name] = [node]
        else:
            seen[node.name].append(node)
    for node_list in seen.values():
        if len(node_list) > 1:
            rv.append([node.to_dict() for node in node_list])


async def purge_file(rv, drive, file_list):
    seen = {}
    for node in file_list:
        if node.name not in seen:
            seen[node.name] = [node]
        elif node.hash_ == seen[node.name][-1].hash_:
            await drive.trash_node(node)
        else:
            seen[node.name].append(node)
    for node_list in seen.values():
        if len(node_list) > 1:
            rv.append([node.to_dict() for node in node_list])


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))

