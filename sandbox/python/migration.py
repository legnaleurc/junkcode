#! /usr/bin/env python3

import asyncio
import mimetypes
import re
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
        new_root_node = await drive.get_node_by_path('/new')
        await migrate_folder(drive, root_node, new_root_node)

        async for root, folders, files in drive.walk(root_node):
            root_path = await drive.get_path(root)
            new_root_path = re.sub(r'^/old', '/new', root_path)
            new_root = await drive.get_node_by_path(new_root_path)
            assert new_root is not None
            print(f'working on {new_root_path}')

            for node in folders:
                print(f'mkdir {node.name}')
                await migrate_folder(drive, node, new_root)
                print('ok')

            for node in files:
                print(f'touch {node.name}')
                await migrate_file(drive, node, new_root)
                print('ok')


async def migrate_folder(drive, node, new_root):
    new_node = await drive.create_folder(new_root, node.name, exist_ok=True)
    while True:
        await asyncio.sleep(1)
        async for change in drive.sync():
            print(change)
        new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
        if new_node:
            break


async def migrate_file(drive, node, new_root):
    new_node = await drive.get_node_by_name_from_parent(node.name, new_root)
    if new_node:
        new_hash = await get_node_hash(drive, node)
        if new_hash == node.hash_:
            return

        await drive.trash_node(new_node)
        async for change in drive.sync():
            print(change)

    while True:
        new_hash, new_node = await copy_node(drive, node, new_root)
        if new_hash == new_node.hash_:
            break

        await drive.trash_node(new_node)
        async for change in drive.sync():
            print(change)


async def get_node_hash(drive, node):
    hasher = await drive.get_hasher()
    async with await drive.download(node) as fin:
        async for chunk in fin:
            hasher.update(chunk)
    rv = hasher.hexdigest()
    return rv


async def copy_node(drive, node, new_root):
    mt, _ = mimetypes.guess_type(node.name)
    hasher = await drive.get_hasher()

    async with \
        await drive.download(node) as fin, \
        await drive.upload(
            new_root,
            node.name,
            node.size,
            mt,
        ) as fout:
        async for chunk in fin:
            hasher.update(chunk)
            await fout.write(chunk)

    new_hash = hasher.hexdigest()
    new_node = await fout.node()
    return new_hash, new_node


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
