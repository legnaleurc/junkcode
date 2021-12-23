import asyncio
from typing import AsyncGenerator, Awaitable, Callable

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
