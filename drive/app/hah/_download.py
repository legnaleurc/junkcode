from concurrent.futures import Executor
from pathlib import Path

from wcpan.queue import AioQueue
from wcpan.drive.core.lib import download_file_to_local
from wcpan.drive.core.types import Drive, Node
from wcpan.drive.cli.lib import get_file_hash


async def download(drive: Drive, src: Node, dst: Path, pool: Executor) -> Path:
    with AioQueue[None].fifo() as queue:
        await queue.push(_download_unknown(queue, drive, src, dst, pool))
        await queue.consume(8)
    return dst / src.name


async def _download_unknown(
    queue: AioQueue[None], drive: Drive, src: Node, dst: Path, pool: Executor
) -> None:
    if src.is_directory:
        await queue.push(_download_directory(queue, drive, src, dst, pool))
    else:
        await queue.push(_download_file(queue, drive, src, dst, pool))


async def _download_directory(
    queue: AioQueue[None], drive: Drive, src: Node, dst: Path, pool: Executor
) -> None:
    name = src.name
    new_direcotry = dst / name
    new_direcotry.mkdir(exist_ok=True, parents=True)
    print(f"mkdir {new_direcotry}")

    children = await drive.get_children(src)
    children = (_ for _ in children if not _.is_trashed)
    for child in children:
        await queue.push(_download_unknown(queue, drive, child, new_direcotry, pool))


async def _download_file(
    queue: AioQueue[None], drive: Drive, src: Node, dst: Path, pool: Executor
) -> None:
    path = await download_file_to_local(drive, src, dst)
    print(f"touch {path}")

    await queue.push(_check_file(drive, src, path, pool))


async def _check_file(drive: Drive, src: Node, tmp: Path, pool: Executor) -> None:
    hash_ = await get_file_hash(tmp, drive=drive, pool=pool)
    assert hash_ == src.hash
    print(f"check {tmp}")
