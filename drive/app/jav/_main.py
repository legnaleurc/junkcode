from argparse import ArgumentParser, Namespace
from collections.abc import Callable, Awaitable, AsyncIterator
from pathlib import Path
import asyncio
import sys

import yaml
from aiohttp import ClientSession
from wcpan.drive.core.types import Drive, Node
from wcpan.logging import ConfigBuilder

from app.lib import create_default_drive

from ._types import ManifestDict, JavData, SauceData
from ._sauce import fetch_jav_data
from ._dispatch import get_jav_query


async def main(args: list[str] | None = None) -> int:
    kwargs = parse_args(args)
    setup_logging()

    action: Callable[[Drive, Namespace], Awaitable[int]] | None = kwargs.action
    if not action:
        return 1

    async with create_default_drive() as drive:
        return await action(drive, kwargs)


def parse_args(args: list[str] | None = None) -> Namespace:
    parser = ArgumentParser("jav")

    command = parser.add_subparsers()

    g_parser = command.add_parser("generate", aliases=["g"])
    g_parser.add_argument("path", type=str)
    g_parser.set_defaults(action=_generate)

    a_parser = command.add_parser("apply", aliases=["a"])
    a_parser.set_defaults(action=_apply)

    f_parser = command.add_parser("filter", aliases=["f"])
    f_parser.set_defaults(action=_filter)

    t_parser = command.add_parser("test", aliases=["t"])
    t_parser.add_argument("--sauce", type=str)
    t_parser.add_argument("--name", type=str)
    t_parser.add_argument("--query", type=str)
    t_parser.set_defaults(action=_test)

    kwargs = parser.parse_args(args)
    return kwargs


def setup_logging():
    from logging.config import dictConfig

    dictConfig(ConfigBuilder().add("app.jav", level="D").to_dict())


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
    manifest: list[ManifestDict] = yaml.safe_load(sys.stdin)
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
    manifest: list[ManifestDict] = yaml.safe_load(sys.stdin)

    not_all_null = (m for m in manifest if any(m["title"].values()))

    def all_same(m: ManifestDict):
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


async def _test(drive: Drive, kwargs: Namespace) -> int:
    jav_data = JavData(
        sauce_list=[
            SauceData(
                sauce=kwargs.sauce,
                name=kwargs.name,
                query=kwargs.query,
            )
        ]
    )
    async with ClientSession() as session:
        title_dict = await fetch_jav_data(session, jav_data)
        print(title_dict)
    return 0


async def _process_node_list(
    session: ClientSession, node_list: list[Node]
) -> AsyncIterator[ManifestDict]:
    for node in node_list:
        if node.is_trashed:
            continue
        jav_query = get_jav_query(node.name)
        if not jav_query:
            continue
        title_dict = await fetch_jav_data(session, jav_query)

        yield {
            "id": node.id,
            "name": node.name,
            "title": title_dict,
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
