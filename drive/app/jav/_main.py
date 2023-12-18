from argparse import ArgumentParser, Namespace
from collections.abc import Callable, Awaitable
from pathlib import Path
from typing import Any
import asyncio
import sys

import yaml
from aiohttp import ClientSession
from wcpan.drive.core.types import Drive, Node
from wcpan.drive.cli.lib import create_drive_from_config

from ._sauce import fetch_jav_data
from ._dispatch import get_jav_query


async def main(args: list[str] | None = None) -> int:
    parser = ArgumentParser("jav")

    parser.add_argument("--drive-path", required=True, type=str)

    command = parser.add_subparsers()

    g_parser = command.add_parser("generate", aliases=["g"])
    g_parser.add_argument("path", type=str)
    g_parser.set_defaults(action=_generate)

    a_parser = command.add_parser("apply", aliases=["a"])
    a_parser.set_defaults(action=_apply)

    f_parser = command.add_parser("filter", aliases=["f"])
    f_parser.set_defaults(action=_filter)

    kwargs = parser.parse_args(args)

    drive_path = Path(kwargs.drive_path).expanduser().resolve()
    action: Callable[[Drive, Namespace], Awaitable[int]] | None = kwargs.action
    if not action:
        return 1

    async with create_drive_from_config(drive_path) as drive:
        return await action(drive, kwargs)


async def _generate(drive: Drive, kwargs: Namespace) -> int:
    root_path = Path(kwargs.path)

    async with ClientSession() as session:
        root_node = await drive.get_node_by_path(root_path)
        children = await drive.get_children(root_node)
        async for node in _process_node_list(session, children):
            yaml.safe_dump(
                [node],
                sys.stdout,
                encoding="utf-8",
                allow_unicode=True,
                default_flow_style=False,
            )
            await asyncio.sleep(1)
    return 0


async def _apply(drive: Drive, kwargs: Namespace) -> int:
    manifest = yaml.safe_load(sys.stdin)
    for row in manifest:
        id_ = row["id"]
        title_dict = row["title"]

        for value in title_dict.values():
            if not value:
                continue

            node = await drive.get_node_by_id(id_)
            print(f"rename {node.name} -> {value}")
            await _rename(drive, node, value)
            break
    return 0


async def _filter(drive: Drive, kwargs: Namespace) -> int:
    manifest = yaml.safe_load(sys.stdin)

    not_all_null = (m for m in manifest if any(m["title"].values()))

    def all_same(m: dict[str, Any]):
        values = set(v for v in m["title"].values() if v)
        values.add(m["name"])
        return len(values) == 1

    not_all_same = (m for m in not_all_null if not all_same(m))

    for node in not_all_same:
        yaml.safe_dump(
            [node],
            sys.stdout,
            encoding="utf-8",
            allow_unicode=True,
            default_flow_style=False,
        )

    return 0


async def _process_node_list(session: ClientSession, node_list: list[Node]):
    for node in node_list:
        if node.is_trashed:
            continue
        jav_query = get_jav_query(node.name)
        if not jav_query:
            continue
        title = await fetch_jav_data(session, jav_query)

        yield {
            "id": node.id,
            "name": node.name,
            "jav_id": jav_query.name,
            "title": title,
        }


async def _rename(drive: Drive, node: Node, new_name: str) -> None:
    if node.is_directory:
        if new_name == node.name:
            print("skipped")
            return
        await drive.move(node, new_parent=None, new_name=new_name)
        return

    if not node.parent_id:
        return

    root_node = await drive.get_node_by_id(node.parent_id)
    parent = await drive.create_directory(new_name, root_node)
    await drive.move(node, new_parent=parent, new_name=None)
