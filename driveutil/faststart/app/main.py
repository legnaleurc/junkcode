#! /usr/bin/env python3

import argparse
import asyncio
import signal
import sys
import tempfile
from concurrent.futures import Executor
from functools import partial
from logging import getLogger
from logging.config import dictConfig
from tempfile import TemporaryDirectory
from typing import NoReturn, Any

from wcpan.drive.core.drive import Drive, DriveFactory
from wcpan.drive.core.types import Node
from wcpan.drive.core.util import create_executor
from wcpan.logging import ConfigBuilder

from .cache import initialize_cache
from .processor import create_processor, is_oggmedia, is_realmedia
from .queue_ import consume, produce, NodeGenerator


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    kwargs = parse_args(args)
    root_path_list: list[str] = kwargs.root_path
    remux_only: bool = kwargs.remux_only
    transcode_only: bool = kwargs.transcode_only
    cache_only: bool = kwargs.cache_only
    jobs: int = kwargs.jobs

    initialize_cache()
    dictConfig(
        ConfigBuilder(path="./data/migrate.log").add("wcpan.drive", "app").to_dict()
    )

    factory = DriveFactory()
    factory.load_config()

    queue = asyncio.Queue(jobs)

    with create_executor() as pool, TemporaryDirectory() as work_folder:
        async with factory(pool=pool) as drive:
            async for change in drive.sync():
                getLogger(__name__).debug(change)

            work = partial(
                node_work,
                drive=drive,
                work_folder=work_folder,
                pool=pool,
                remux_only=remux_only,
                transcode_only=transcode_only,
                cache_only=cache_only,
            )
            consumer_list = [
                asyncio.create_task(consume(queue, work), name=f"consumer-{i}")
                for i in range(jobs)
            ]

            producer = asyncio.create_task(
                until_finished(
                    queue,
                    walk_root_list(drive, root_path_list),
                    consumer_list,
                ),
                name=f"producer",
            )

            loop = asyncio.get_running_loop()
            loop.add_signal_handler(
                signal.SIGINT,
                partial(
                    shutdown,
                    tasks=[
                        producer,
                        *consumer_list,
                    ],
                ),
            )

            await asyncio.gather(
                producer,
                *consumer_list,
                return_exceptions=True,
            )

    return 0


def parse_args(args: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser("app")

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--remux-only", action="store_true", default=False)
    mutex_group.add_argument("--transcode-only", action="store_true", default=False)
    mutex_group.add_argument("--cache-only", action="store_true", default=False)

    parser.add_argument("--jobs", "-j", type=int, default=1)

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


async def until_finished(
    queue: asyncio.Queue,
    node_gen: NodeGenerator,
    consumer_list: list[asyncio.Task[NoReturn]],
):
    await produce(queue, node_gen)
    await queue.join()
    cancel_tasks(consumer_list)


def shutdown(*, tasks: list[asyncio.Task[Any]]):
    cancel_tasks(tasks)
    for task in tasks:
        if not task.done():
            task.print_stack()
        elif task.cancelled():
            print(f"{task.get_name()} cancelled")


def cancel_tasks(tasks: list[asyncio.Task[Any]]):
    for task in tasks:
        if not task.done():
            task.cancel()
