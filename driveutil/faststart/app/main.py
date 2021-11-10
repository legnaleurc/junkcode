#! /usr/bin/env python3

import argparse
import sys
import tempfile

from wcpan.drive.core.drive import DriveFactory
from wcpan.logger import setup as setup_logger, WARNING, DEBUG

from .cache import initialize_cache
from .processor import create_processor


async def main(args: list[str] = None):
    if args is None:
        args = sys.argv

    kwargs = parse_args(args)
    root_path: str = kwargs.root_path
    remux_only: bool = kwargs.remux_only
    transcode_only: bool = kwargs.transcode_only

    initialize_cache()
    setup_logger([
        'wcpan.drive',
        'faststart',
    ], './data/migrate.log')

    factory = DriveFactory()
    factory.load_config()

    async with factory() as drive:
        async for change in drive.sync():
            DEBUG('faststart') << change

        root_node = await drive.get_node_by_path(root_path)
        assert root_node is not None

        with tempfile.TemporaryDirectory() as work_folder:
            async for root, folders, files in drive.walk(root_node):
                for file_ in files:
                    if not file_.is_video:
                        continue

                    processor = create_processor(work_folder, drive, file_)
                    if not processor:
                        WARNING('faststart') << 'no processor for' << file_.name
                        continue

                    await processor(remux_only=remux_only, transcode_only=transcode_only)

    return 0


def parse_args(args: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser('app')

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument('--remux-only', action='store_true', default=False)
    mutex_group.add_argument('--transcode-only', action='store_true', default=False)

    parser.add_argument('root_path', type=str)

    return parser.parse_args(args[1:])
