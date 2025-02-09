from argparse import ArgumentParser, Namespace
from collections.abc import Callable, Awaitable, AsyncIterator
from pathlib import Path
from typing import Sequence, TypedDict
import asyncio
import sys

import yaml
from wcpan.drive.core.types import Drive, Node
from wcpan.logging import ConfigBuilder
from wcpan.jav import generate_detailed_products

from app.lib import create_default_drive


class ManifestDict(TypedDict):
    id: str
    name: str
    title: dict[str, str]


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

    kwargs = parser.parse_args(args)
    return kwargs


def setup_logging():
    from logging.config import dictConfig

    dictConfig(ConfigBuilder().add("app.jav", level="D").to_dict())


async def _generate(drive: Drive, kwargs: Namespace) -> int:
    root_path = Path(kwargs.path)
    root_node = await drive.get_node_by_path(root_path)
    children = await drive.get_children(root_node)
    async for node in _process_node_list(children):
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


async def _process_node_list(node_list: list[Node]) -> AsyncIterator[ManifestDict]:
    for node in node_list:
        if node.is_trashed:
            continue

        pairs = [_ async for _ in _normalize_titles(node.name)]
        if not pairs:
            continue

        title_dict = dict(_pad_keys(pairs))
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


async def _normalize_titles(name: str) -> AsyncIterator[tuple[str, str]]:
    async for product in generate_detailed_products(name):
        match product.sauce:
            case "fanza" | "mgs" | "fc2" | "heyzo":
                title = f"{product.id} {product.title}"
                yield product.sauce, title
            case "carib" | "caribpr" | "1pondo" | "10musume":
                actresses = " ".join(product.actresses)
                title = f"{product.id} {product.title} {actresses}"
                yield product.sauce, title
            case _:
                yield product.sauce, product.title


def _pad_keys[V](pairs: Sequence[tuple[str, V]]) -> list[tuple[str, V]]:
    if not pairs:
        return []

    ml = max(len(_[0]) for _ in pairs)
    padded = [(f"{k:_>{ml}}", v) for k, v in pairs]
    return padded


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
