import asyncio as aio
import argparse
import signal
import sys

from .watcher import Watcher


async def main(args=None):
    if args is None:
        args = sys.argv[1:]

    args = parse_args(args)

    loop = aio.get_running_loop()
    stop_event = aio.Event()
    loop.add_signal_handler(signal.SIGINT, lambda: stop_event.set())

    async with Watcher() as watcher:
        async for changes in watcher('/tmp', stop_event=stop_event):
            print(changes)

    print('leave')

    return 0


def parse_args(args):
    parser = argparse.ArgumentParser('wcpan.watchdog')

    parser.add_argument('--include', '-i', action='append')
    parser.add_argument('--exclude', '-e', action='append')

    rest = None
    try:
        i = args.index('--')
        rest = args[i+1:]
        args = args[:i]
        args = parser.parse_args(args)
    except ValueError:
        args = parser.parse_args(args)

    return args, rest
