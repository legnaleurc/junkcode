#! /usr/bin/env python3

import asyncio
import re
import sys

import aiohttp
from bs4 import BeautifulSoup
from wcpan.drive.core.drive import DriveFactory, Drive, Node


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    root_path = args[1]

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive, \
               aiohttp.ClientSession() as session:
        root_node = await drive.get_node_by_path(root_path)
        children = await drive.get_children(root_node)
        for child in children:
            jav_id = get_jav_id(child.name)
            if not jav_id:
                continue
            title = await fetch_jav_data(session, jav_id)
            if not title:
                continue
            title = normalize_title(title)
            if title == child.name:
                continue

            print(f'old: {child.name}')
            print(f'new: {title}')
            confirm = input('y/N')
            if confirm != 'y':
                continue
            await rename(drive, child, title)

    return 0


def get_jav_id(name: str) -> str:
    rv = re.match(r'([A-Za-z0-9]+-[0-9]+)', name)
    if not rv:
        return ''
    rv = rv.group(1)
    rv = rv.upper()
    return rv


async def fetch_jav_data(session: aiohttp.ClientSession, jav_id: str):
    async with session.get(f'https://www.javbus.com/ja/{jav_id}') as response:
        if response.status != 200:
            return None
        html = await response.text()
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('.container > h3')
        if not title:
            return None
        return title.text.strip()


def normalize_title(title: str) -> str:
    return title.replace('/', '／')


async def rename(drive: Drive, node: Node, new_name: str):
    if node.is_folder:
        await drive.rename_node(node, new_parent=None, new_name=new_name)
        return
    root_node = await drive.get_node_by_id(node.parent_id)
    parent = await drive.create_folder(root_node, new_name)
    await drive.rename_node(node, new_parent=parent, new_name=None)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
