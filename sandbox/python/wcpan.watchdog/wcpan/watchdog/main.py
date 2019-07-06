import asyncio as aio
import argparse
import functools
import signal
import sys

from .watcher import Watcher
from .filters import create_default_filter, matches_glob


async def main(args=None):
    if args is None:
        args = sys.argv[1:]

    args, rest = parse_args(args)
    filter_ = create_filter(args)

    loop = aio.get_running_loop()
    stop_event = aio.Event()
    loop.add_signal_handler(signal.SIGINT, lambda: stop_event.set())

    async with ChildProcess(rest) as child, \
               Watcher() as watcher:
        async for changes in watcher(args.path, stop_event=stop_event,
                                     filter_=filter_):
            print(changes)

            await child.restart()

    return 0


def parse_args(args):
    parser = argparse.ArgumentParser('wcpan.watchdog')

    parser.add_argument('--include', '-i', action='append')
    parser.add_argument('--exclude', '-e', action='append')
    parser.add_argument('path', nargs='?', type=str, default='.')

    rest = None
    try:
        i = args.index('--')
        rest = args[i+1:]
        args = args[:i]
        args = parser.parse_args(args)
    except ValueError:
        args = parser.parse_args(args)

    return args, rest


def create_filter(args):
    filter_ = create_default_filter()
    if args.include:
        for p in args.include:
            filter_.include(matches_glob(p))
    if args.exclude:
        for p in args.exclude:
            filter_.exclude(matches_glob(p))
    return filter_


class ChildProcess(object):

    def __init__(self, args):
        self._args = args
        self._p = None

    async def __aenter__(self):
        if not self._args:
            return self
        self._p = await spawn(self._args)
        return self

    async def __aexit__(self, type_, e, tb):
        if not self._args:
            return
        await kill(self._p)

    async def restart(self):
        if not self._args:
            return
        await kill(self._p)
        self._p = await spawn(self._args)


async def spawn(args):
    p = await aio.create_subprocess_exec(*args)
    return p


async def kill(p):
    if p.returncode is not None:
        return

    p.terminate()
    try:
        return await aio.wait_for(p.wait(), timeout=2)
    except aio.TimeoutError:
        pass

    p.kill()
    return await p.wait()
