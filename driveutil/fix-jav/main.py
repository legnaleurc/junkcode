import asyncio
import re
import sys

import aiohttp
import yaml
from wcpan.drive.core.drive import DriveFactory, Drive, Node

from .sauce import fetch_jav_data
from .dispatch import get_jav_id


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    pattern = args[1]
    root_path = args[2]

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive, \
               aiohttp.ClientSession() as session:
        root_node = await drive.get_node_by_path(root_path)
        children = await drive.get_children(root_node)
        async for node in process_node_list(session, pattern, children):
            yaml.safe_dump([node], sys.stdout, encoding='utf-8', allow_unicode=True, default_flow_style=False)
            await asyncio.sleep(1)

    return 0


async def process_node_list(session: aiohttp.ClientSession, pattern: str, node_list: list[Node]):
    for node in node_list:
        if node.trashed:
            continue
        jav_id = get_jav_id(pattern, node.name)
        if not jav_id:
            continue
        title = await fetch_jav_data(session, jav_id)
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
