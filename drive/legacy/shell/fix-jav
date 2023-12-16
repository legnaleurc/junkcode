#! /usr/bin/env python3

import asyncio
import functools
import re
import sys

import aiohttp
import yaml
from bs4 import BeautifulSoup
from wcpan.drive.core.drive import DriveFactory, Drive, Node


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


def get_jav_id(pattern: str, name: str) -> str:
    rv = re.search(pattern, name)
    if not rv:
        return ''
    rv = rv.group(0)
    rv = rv.upper()
    return rv


def named_fetch(name: str):
    def decorator(fn):
        @functools.wraps(fn)
        async def wrapper(*args, **kwargs):
            rv = await fn(*args, **kwargs)
            return name, rv
        return wrapper
    return decorator


@named_fetch('javbus')
async def fetch_jav_data_from_javbus(session: aiohttp.ClientSession, jav_id: str):
    async with session.get(f'https://www.javbus.com/ja/{jav_id}') as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('.container > h3')
        if not title:
            return None
        return normalize_title(title.text)


@named_fetch('javlibrary')
async def fetch_jav_data_from_javlibrary(session: aiohttp.ClientSession, jav_id: str):
    async with session.get(
        f'http://www.javlibrary.com/ja/vl_searchbyid.php',
        params={
            'keyword': jav_id,
        },
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('#video_title .post-title')
        if not title:
            return None
        return normalize_title(title.text)


SOURCE_LIST = [
    fetch_jav_data_from_javbus,
    fetch_jav_data_from_javlibrary,
]


async def fetch_jav_data(session: aiohttp.ClientSession, jav_id: str):
    tasks = [source(session, jav_id) for source in SOURCE_LIST]
    tmp = await asyncio.gather(*tasks)
    rv = {name: value for name, value in tmp}
    return rv


def normalize_title(title: str) -> str:
    title = title.strip()
    title = title.replace('/', '／')
    title = title.replace('\n', '')
    title = title.replace('\r', '')
    return title


async def rename(drive: Drive, node: Node, new_name: str):
    if node.is_folder:
        await drive.rename_node(node, new_parent=None, new_name=new_name)
        return
    root_node = await drive.get_node_by_id(node.parent_id)
    parent = await drive.create_folder(root_node, new_name)
    await drive.rename_node(node, new_parent=parent, new_name=None)


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
