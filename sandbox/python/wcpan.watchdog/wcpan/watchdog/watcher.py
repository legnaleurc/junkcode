import asyncio as aio
import concurrent.futures as cf
import contextlib as cl
import functools as ft
import time

from .walker import Walker
from .filters import create_default_filter


class Watcher(object):
    """
    Creates a filesystem watcher.

    stop_event is an asyncio.Event object which gives the watcher a hint about
    when to stop the watching loop. If stop_event is None, the loop will not
    stop.

    filter_ is a Filter object, to filter out files and directories being
    watching. If filter_ is None, create_default_filter() will be used.
    """

    def __init__(self,
        stop_event=None,
        filter_=None,
        min_sleep=50,
        normal_sleep=400,
        debounce=1600,
        executor=None,
        loop=None,
    ):
        self._stop_event = stop_event
        self._filter = filter_
        self._min_sleep = min_sleep
        self._normal_sleep = normal_sleep
        self._debounce = debounce
        self._executor = executor
        self._loop = loop
        if not self._loop:
            self._loop = aio.get_running_loop()

    async def __aenter__(self):
        async with cl.AsyncExitStack() as stack:
            if self._executor is None:
                self._executor = stack.enter_context(cf.ThreadPoolExecutor())
            self._raii = stack.pop_all()
        return WatcherCreator(self)

    async def __aexit__(self, type_, exc, tb):
        await self._raii.aclose()

    async def _run(self, cb, *args, **kwargs):
        fn = ft.partial(cb, *args, **kwargs)
        return await self._loop.run_in_executor(self._executor, fn)

    async def _sleep(self, sec):
        await aio.sleep(sec, loop=self._loop)


class WatcherCreator(object):

    def __init__(self, context):
        self._context = context

    def __call__(self,
        path,
        stop_event=None,
        filter_=None,
        min_sleep=None,
        normal_sleep=None,
        debounce=None,
    ):
        if stop_event is None:
            stop_event = self._context._stop_event
        if filter_ is None:
            filter_ = create_default_filter()
        if min_sleep is None:
            min_sleep = self._context._min_sleep
        if normal_sleep is None:
            normal_sleep = self._context._normal_sleep
        if debounce is None:
            debounce = self._context._debounce

        return ChangeIterator(
            run=self._context._run,
            sleep=self._context._sleep,
            path=path,
            stop_event=stop_event,
            filter_=filter_,
            min_sleep=min_sleep,
            normal_sleep=normal_sleep,
            debounce=debounce,
        )


class ChangeIterator(object):

    def __init__(self,
        run,
        sleep,
        path,
        stop_event,
        filter_,
        min_sleep,
        normal_sleep,
        debounce,
    ):
        self._run = run
        self._sleep = sleep
        self._path = path
        self._stop_event = stop_event
        self._filter = filter_
        self._min_sleep = min_sleep
        self._normal_sleep = normal_sleep
        self._debounce = debounce

        self._walker = None

    def __aiter__(self):
        return self

    async def __anext__(self):
        if not self._walker:
            self._walker = Walker(self._filter, self._path)
            await self._run(self._walker)

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
                await self._sleep(sleep_time / 1000)

            s = unix_ms()
            new_changes = await self._run(self._walker)
            changes.update(new_changes)

            now = unix_ms()
            check_time = now - s
            debounced = now - last_change

            if changes and (not new_changes or debounced > self._debounce):
                return changes


def unix_ms():
    return int(round(time.time() * 1000))
