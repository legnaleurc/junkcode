import asyncio
import sys

import yaml
from aiohttp import ClientSession
from wcpan.drive.core.drive import DriveFactory, Drive, Node

from .sauce import fetch_jav_data
from .dispatch import get_jav_id


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    await produce_manifest(args)
    # await apply_manifest(args)

    return 0


async def produce_manifest(args: list[str]):
    root_path = args[1]

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive, \
               ClientSession() as session:
        root_node = await drive.get_node_by_path(root_path)
        children = await drive.get_children(root_node)
        async for node in process_node_list(session, children):
            yaml.safe_dump(
                [node],
                sys.stdout,
                encoding='utf-8',
                allow_unicode=True,
                default_flow_style=False,
            )
            await asyncio.sleep(1)


async def apply_manifest(args: list[str]):
    factory = DriveFactory()
    factory.load_config()

    manifest = yaml.safe_load(sys.stdin)
    async with factory() as drive:
        for row in manifest:
            id_ = row['id']
            title_dict = row['title']

            for value in title_dict.values():
                if not value:
                    continue

                node = await drive.get_node_by_id(id_)
                print(f'rename {node.name} -> {value}')
                await rename(drive, node, value)
                break


async def process_node_list(session: ClientSession, node_list: list[Node]):
    for node in node_list:
        if node.trashed:
            continue
        jav_id = get_jav_id(node.name)
        if not jav_id:
            continue
        title = await fetch_jav_data(session, jav_id)

        name_set = set()
        name_set.add(node.name)
        for value in title.values():
            name_set.add(value)
        if len(name_set) <= 1:
            continue

        yield {
            'id': node.id_,
            'name': node.name,
            'jav_id': jav_id,
            'title': title,
        }


async def rename(drive: Drive, node: Node, new_name: str):
    if node.is_folder:
        await drive.rename_node(node, new_parent=None, new_name=new_name)
        return
    root_node = await drive.get_node_by_id(node.parent_id)
    parent = await drive.create_folder(root_node, new_name)
    await drive.rename_node(node, new_parent=parent, new_name=None)
