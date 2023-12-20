#! /usr/bin/env python3

import asyncio
import sys
from argparse import ArgumentParser, Namespace
from concurrent.futures import Executor
from contextlib import AsyncExitStack
from logging import getLogger
from logging.config import dictConfig
from pathlib import PurePath, Path
from tempfile import TemporaryDirectory

from wcpan.drive.core.types import Node, Drive
from wcpan.drive.cli.lib import create_executor
from wcpan.logging import ConfigBuilder
from wcpan.queue import AioQueue

from app.lib import create_default_drive

from .cache import initialize_cache
from .processor import create_processor, is_oggmedia, is_realmedia


async def main(args: list[str] | None = None):
    kwargs = parse_args(args)
    data_path = Path(kwargs.data_path).expanduser().resolve()
    root_path_list = [PurePath(_) for _ in kwargs.root_path]
    remux_only: bool = kwargs.remux_only
    transcode_only: bool = kwargs.transcode_only
    cache_only: bool = kwargs.cache_only
    jobs: int = kwargs.jobs
    tmp_path: str | None = kwargs.tmp_path

    data_path.mkdir(exist_ok=True, parents=True)

    dsn = data_path / "_migrated.sqlite"
    initialize_cache(dsn)
    dictConfig(
        ConfigBuilder(path=data_path / "migrate.log")
        .add("wcpan")
        .add("app", level="D")
        .to_dict()
    )

    async with AsyncExitStack() as stack:
        pool = stack.enter_context(create_executor())
        work_folder = Path(stack.enter_context(TemporaryDirectory(dir=tmp_path)))
        queue = stack.enter_context(AioQueue[None].fifo())
        drive = await stack.enter_async_context(create_default_drive())

        async for change in drive.sync():
            getLogger(__name__).debug(change)

        async for video_file in walk_root_list(drive, root_path_list):
            await queue.push(
                node_work(
                    video_file,
                    drive=drive,
                    work_folder=work_folder,
                    dsn=dsn,
                    pool=pool,
                    remux_only=remux_only,
                    transcode_only=transcode_only,
                    cache_only=cache_only,
                )
            )

        await queue.consume(jobs)

    return 0


def parse_args(args: list[str] | None) -> Namespace:
    if args is None:
        args = sys.argv

    parser = ArgumentParser("app")

    parser.add_argument("--drive-path", required=True, type=str)
    parser.add_argument("--data-path", required=True, type=str)
    parser.add_argument("--tmp-path", type=str)
    parser.add_argument("--jobs", "-j", default=1)

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--remux-only", action="store_true", default=False)
    mutex_group.add_argument("--transcode-only", action="store_true", default=False)
    mutex_group.add_argument("--cache-only", action="store_true", default=False)

    parser.add_argument("root_path", type=str, nargs="+")

    return parser.parse_args(args[1:])


async def walk_root_list(drive: Drive, root_list: list[PurePath]):
    for root_path in root_list:
        root_node = await drive.get_node_by_path(root_path)
        # TODO add log
        if not root_node:
            continue

        async for _root, _folders, files in drive.walk(root_node):
            for file_ in files:
                if (
                    not file_.is_video
                    and not is_realmedia(file_)
                    and not is_oggmedia(file_)
                ):
                    continue
                yield file_


async def node_work(
    node: Node,
    *,
    drive: Drive,
    work_folder: Path,
    dsn: Path,
    pool: Executor,
    remux_only: bool,
    transcode_only: bool,
    cache_only: bool,
):
    getLogger(__name__).info(f"processing {node.name}")
    processor = create_processor(
        work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
    )
    if not processor:
        getLogger(__name__).warning(f"no processor for {node.name}")
        return

    used_quota = await processor(
        remux_only=remux_only,
        transcode_only=transcode_only,
        cache_only=cache_only,
    )
    if used_quota:
        await asyncio.sleep(60)
