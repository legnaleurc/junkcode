#! /usr/bin/env python3


import asyncio as aio
import concurrent.futures as cf
import contextlib as cl
import functools as ft
import sys
import time

import signal


class Watcher(object):

    def __init__(self, executor=None, loop=None):
        self._executor = executor
        self._loop = loop
        if not self._loop:
            self._loop = aio.get_running_loop()

    async def __aenter__(self):
        async with cl.AsyncExitStack() as stack:
            if self._executor is None:
                self._executor = stack.enter_context(cf.ThreadPoolExecutor())
            self._raii = stack.pop_all()
        return WatcherCreator(self._run)

    async def __aexit__(self, type_, exc, tb):
        await self._raii.aclose()

    async def _run(self, cb, *args, **kwargs):
        fn = ft.partial(cb, *args, **kwargs)
        return await self._loop.run_in_executor(self._executor, fn)


class WatcherCreator(object):

    def __init__(self, runner):
        self._runner = runner

    def __call__(self, path, stop_event=None):
        return ChangeIterator(self._runner, path, stop_event)


class ChangeIterator(object):

    def __init__(self, runner, path, stop_event):
        self._runner = runner
        self._path = path
        self._stop_event = stop_event

        self._min_sleep = 50
        self._normal_sleep = 400
        self._debounce = 1600

    def __aiter__(self):
        return self

    async def __anext__(self):
        check_time = 0
        changes = set()
        last_change = 0
        while True:
            if self._stop_event and self._stop_event.is_set():
                raise StopAsyncIteration

            if not changes:
                last_change = unix_ms()

            if check_time:
                if changes:
                    sleep_time = self._min_sleep
                else:
                    sleep_time = max(self._normal_sleep - check_time, self._min_sleep)
                await aio.sleep(sleep_time / 1000)

            s = unix_ms()
            new_changes = await self._runner(lambda: [123])
            changes.update(new_changes)

            now = unix_ms()
            check_time = now - s
            debounced = now - last_change

            if changes and (not new_changes or debounced > self._debounce):
                return changes


def unix_ms():
    return int(round(time.time() * 1000))


async def main(args=None):
    if args is None:
        args = sys.argv

    loop = aio.get_running_loop()
    stop_event = aio.Event()
    loop.add_signal_handler(signal.SIGINT, lambda: stop_event.set())

    async with Watcher() as watcher:
        async for changes in watcher('/tmp', stop_event=stop_event):
            print(changes)

    print('leave')

    return 0


if __name__ == '__main__':
    sys.exit(aio.run(main()))
