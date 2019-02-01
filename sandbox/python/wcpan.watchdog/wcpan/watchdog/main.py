import asyncio as aio
import signal
import sys

from .watcher import Watcher


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
