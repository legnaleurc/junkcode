import asyncio
from typing import AsyncGenerator, Awaitable, Callable, NoReturn

from wcpan.drive.core.types import Node


NodeGenerator = AsyncGenerator[Node, None]
NodeProcessor = Callable[[Node], Awaitable[None]]


async def produce(queue: asyncio.Queue, node_gen: NodeGenerator):
    async for node in node_gen:
        await queue.put(node)


async def consume(queue: asyncio.Queue, fn: NodeProcessor):
    while True:
        node: Node = await queue.get()
        try:
            await fn(node)
        finally:
            queue.task_done()


async def until_finished(
    queue: asyncio.Queue,
    node_gen: NodeGenerator,
    consumer_list: list[asyncio.Task[NoReturn]],
):
    await produce(queue, node_gen)
    await queue.join()
    for task in consumer_list:
        if not task.done():
            task.cancel()
