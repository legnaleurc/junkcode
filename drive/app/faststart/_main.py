#! /usr/bin/env python3

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

from ._args import parse_args
from ._cache import initialize_cache
from ._processor import create_processor


_L = getLogger(__name__)


async def main(args: list[str]):
    kwargs = parse_args(args)

    kwargs.data_path.mkdir(exist_ok=True, parents=True)

    dsn = kwargs.data_path / "_migrated.sqlite"
    initialize_cache(dsn)
    dictConfig(
        ConfigBuilder(path=kwargs.data_path / "migrate.log")
        .add("wcpan")
        .add("app", level="D")
        .to_dict()
    )

    async with AsyncExitStack() as stack:
        pool = stack.enter_context(create_executor())
        work_folder = Path(stack.enter_context(TemporaryDirectory(dir=kwargs.tmp_path)))
        queue = stack.enter_context(AioQueue[None].fifo())
        drive = await stack.enter_async_context(create_default_drive())

        async for change in drive.sync():
            _L.debug(change)

        async for file_ in walk_root_list(drive, kwargs.root_path_list):
            await queue.push(
                node_work(
                    file_,
                    drive=drive,
                    work_folder=work_folder,
                    dsn=dsn,
                    pool=pool,
                    remux_only=kwargs.remux_only,
                    transcode_only=kwargs.transcode_only,
                    cache_only=kwargs.cache_only,
                )
            )

        await queue.consume(kwargs.jobs)

    return 0


async def walk_root_list(drive: Drive, root_list: list[PurePath]):
    for root_path in root_list:
        root_node = await drive.get_node_by_path(root_path)
        # TODO add log
        if not root_node:
            continue

        async for _root, _folders, files in drive.walk(root_node):
            for file_ in files:
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
    processor = create_processor(
        work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
    )
    if not processor:
        return

    _L.info(f"begin {node.name}")
    used_quota = await processor(
        remux_only=remux_only,
        transcode_only=transcode_only,
        cache_only=cache_only,
    )
    _L.info(f"used quota {used_quota}, end {node.name}")
