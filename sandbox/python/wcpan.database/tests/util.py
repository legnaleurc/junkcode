import asyncio


def await_(f):
    loop = asyncio.get_event_loop()
    return loop.run_until_complete(f)
