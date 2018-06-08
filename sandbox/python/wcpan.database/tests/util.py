import asyncio
import functools as ft


def sync(method):
    @ft.wraps(method)
    def wrapper(self, *args, **kwargs):
        loop = asyncio.get_event_loop()
        f = method(self, *args, **kwargs)
        return loop.run_until_complete(f)
    return wrapper
