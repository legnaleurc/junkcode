#! /usr/bin/env python3

import argparse
import asyncio
import sys
from concurrent.futures import Executor
from logging import getLogger
from logging.config import dictConfig
from tempfile import TemporaryDirectory

from wcpan.drive.core.drive import Drive, DriveFactory
from wcpan.drive.core.types import Node
from wcpan.drive.core.util import create_executor
from wcpan.logging import ConfigBuilder
from wcpan.queue import AioQueue

from .cache import initialize_cache
from .processor import create_processor, is_oggmedia, is_realmedia


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    kwargs = parse_args(args)
    root_path_list: list[str] = kwargs.root_path
    remux_only: bool = kwargs.remux_only
    transcode_only: bool = kwargs.transcode_only
    cache_only: bool = kwargs.cache_only
    jobs: int = kwargs.jobs
    tmp_path: str | None = kwargs.tmp_path

    initialize_cache()
    dictConfig(
        ConfigBuilder(path="./data/migrate.log")
        .add("wcpan")
        .add("app", level="D")
        .to_dict()
    )

    factory = DriveFactory()
    factory.load_config()

    with create_executor() as pool, TemporaryDirectory(
        dir=tmp_path
    ) as work_folder, AioQueue.fifo() as queue:
        async with factory(pool=pool) as drive:
            async for change in drive.sync():
                getLogger(__name__).debug(change)

            async for video_file in walk_root_list(drive, root_path_list):
                await queue.push(
                    node_work(
                        video_file,
                        drive=drive,
                        work_folder=work_folder,
                        pool=pool,
                        remux_only=remux_only,
                        transcode_only=transcode_only,
                        cache_only=cache_only,
                    )
                )

            await queue.consume(jobs)

    return 0


def parse_args(args: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser("app")

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--remux-only", action="store_true", default=False)
    mutex_group.add_argument("--transcode-only", action="store_true", default=False)
    mutex_group.add_argument("--cache-only", action="store_true", default=False)

    parser.add_argument("--jobs", "-j", type=int, default=1)
    parser.add_argument("--tmp-path", type=str)

    parser.add_argument("root_path", type=str, nargs="+")

    return parser.parse_args(args[1:])


async def walk_root_list(drive: Drive, root_list: list[str]):
    for root_path in root_list:
        root_node = await drive.get_node_by_path(root_path)
        # TODO add log
        if not root_node:
            continue

        async for root, folders, files in drive.walk(root_node):
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
    work_folder: str,
    pool: Executor,
    remux_only: bool,
    transcode_only: bool,
    cache_only: bool,
):
    getLogger(__name__).info(f"processing {node.name}")
    processor = create_processor(work_folder, pool, drive, node)
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
