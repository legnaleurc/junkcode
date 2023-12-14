from pathlib import Path

from wcpan.queue import AioQueue
from wcpan.drive.core.lib import download_file_to_local
from wcpan.drive.core.types import Drive, Node


async def download(drive: Drive, src: Node, dst: Path) -> Path:
    with AioQueue[None].fifo() as queue:
        await queue.push(_download_unknown(queue, drive, src, dst))
        await queue.consume(4)
    return dst / src.name


async def _download_unknown(
    queue: AioQueue[None], drive: Drive, src: Node, dst: Path
) -> None:
    if src.is_directory:
        await queue.push(_download_directory(queue, drive, src, dst))
    else:
        await queue.push(_download_file(drive, src, dst))


async def _download_directory(
    queue: AioQueue[None], drive: Drive, src: Node, dst: Path
) -> None:
    name = src.name
    new_direcotry = dst / name
    new_direcotry.mkdir(exist_ok=True, parents=True)
    print(f"mkdir {new_direcotry}")

    children = await drive.get_children(src)
    children = (_ for _ in children if not _.is_trashed)
    for child in children:
        await queue.push(_download_unknown(queue, drive, child, new_direcotry))


async def _download_file(drive: Drive, src: Node, dst: Path) -> None:
    path = await download_file_to_local(drive, src, dst)
    print(f"touch {path}")
