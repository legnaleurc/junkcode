from asyncio import get_running_loop, run
from concurrent.futures import Executor
from pathlib import Path

from wcpan.drive.core.types import CreateHasher


async def get_hash(path: Path, factory: CreateHasher, pool: Executor) -> str:
    loop = get_running_loop()
    return await loop.run_in_executor(pool, _get_hash, factory, path)


def _get_hash(factory: CreateHasher, path: Path) -> str:
    async def _run():
        hasher = await factory()
        with path.open("rb") as fin:
            for chunk in fin:
                await hasher.update(chunk)
        return await hasher.hexdigest()

    return run(_run())
