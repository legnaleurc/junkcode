#! /usr/bin/env python3

import argparse
import asyncio
import functools
import sys
import tempfile
from typing import NoReturn

from wcpan.drive.core.drive import Drive, DriveFactory
from wcpan.drive.core.types import Node
from wcpan.logger import setup as setup_logger, WARNING, DEBUG

from .cache import initialize_cache
from .processor import create_processor
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
    setup_logger([
        'wcpan.drive',
        'faststart',
    ], './data/migrate.log')

    factory = DriveFactory()
    factory.load_config()

    queue = asyncio.Queue(jobs)

    async with factory() as drive:
        async for change in drive.sync():
            DEBUG('faststart') << change

        with tempfile.TemporaryDirectory() as work_folder:
            work = functools.partial(node_work,
                drive=drive,
                work_folder=work_folder,
                remux_only=remux_only,
                transcode_only=transcode_only,
                cache_only=cache_only,
            )
            consumer_list = [
                asyncio.create_task(consume(queue, work), name=f'consumer-{i}')
                for i in range(jobs)
            ]

            producer = asyncio.create_task(
                until_finished(
                    queue,
                    walk_root_list(drive, root_path_list),
                    consumer_list,
                ),
                name=f'producer',
            )

            await asyncio.gather(
                producer,
                *consumer_list,
                return_exceptions=True,
            )

    return 0


def parse_args(args: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser('app')

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument('--remux-only', action='store_true', default=False)
    mutex_group.add_argument('--transcode-only', action='store_true', default=False)
    mutex_group.add_argument('--cache-only', action='store_true', default=False)

    parser.add_argument('--jobs', '-j', type=int, default=1)

    parser.add_argument('root_path', type=str, nargs='+')

    return parser.parse_args(args[1:])


async def walk_root_list(drive: Drive, root_list: list[str]):
    for root_path in root_list:
        root_node = await drive.get_node_by_path(root_path)
        # TODO add log
        if not root_node:
            continue

        async for root, folders, files in drive.walk(root_node):
            for file_ in files:
                if not file_.is_video:
                    continue
                yield file_


async def node_work(
    node: Node,
    *,
    drive: Drive,
    work_folder: str,
    remux_only: bool,
    transcode_only: bool,
    cache_only: bool,
):
    processor = create_processor(work_folder, drive, node)
    if not processor:
        WARNING('faststart') << 'no processor for' << node.name
        return

    await processor(
        remux_only=remux_only,
        transcode_only=transcode_only,
        cache_only=cache_only,
    )


async def until_finished(
    queue: asyncio.Queue,
    node_gen: NodeGenerator,
    consumer_list: list[asyncio.Task[NoReturn]],
):
    await produce(queue, node_gen)
    await queue.join()
    for task in consumer_list:
        if not task.done():
            task.cancel()
