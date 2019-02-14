import asyncio as aio
import argparse
import fnmatch
import signal
import sys

from .watcher import Watcher
from .filters import DefaultFilter


class GlobFilter(DefaultFilter):

    def __init__(self, root_path, include_list, exclude_list):
        super().__init__(root_path)

        self._i = include_list
        self._e = exclude_list

    def should_watch_dir(self, entry):
        rv = self._should_watch(entry)
        if rv is not None:
            return rv

        if super().should_watch_dir(entry):
            return True

        return False

    def should_watch_file(self, entry):
        if super().should_watch_file(entry):
            return True
        return False

    def _should_watch(self, entry):
        if self._e:
            excluded = any((fnmatch.fnmatch(entry.name, _) for _ in self._e))
            if excluded:
                return False

        if self._i:
            included = any((fnmatch.fnmatch(entry.name, _) for _ in self._i))
            if included:
                return True

        return None


async def main(args=None):
    if args is None:
        args = sys.argv[1:]

    args, rest = parse_args(args)
    Filter = create_filter_class(args)

    loop = aio.get_running_loop()
    stop_event = aio.Event()
    loop.add_signal_handler(signal.SIGINT, lambda: stop_event.set())

    p = await spawn(rest)

    async with Watcher() as watcher:
        async for changes in watcher(args.path, stop_event=stop_event,
                                     filter_class=Filter):
            print(changes)

            await kill(p)
            p = await spawn(rest)

    await kill(p)

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


def create_filter_class(args):
    if not args.include and not args.exclude:
        return DefaultFilter


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
