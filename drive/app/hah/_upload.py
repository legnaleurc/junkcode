from asyncio import TaskGroup
from concurrent.futures import Executor
from pathlib import Path
from mimetypes import guess_type

from wcpan.drive.core.types import Drive, Node
from wcpan.drive.core.lib import upload_file_from_local
from wcpan.drive.core.exceptions import NodeNotFoundError
from wcpan.drive.cli.lib import get_file_hash


async def upload(drive: Drive, src: Path, dst: Node, pool: Executor) -> None:
    async with TaskGroup() as group:
        check = group.create_task(get_file_hash(src, drive=drive, pool=pool))
        upload_ = group.create_task(_maybe_upload(drive, src, dst))
        hash_ = await check
        node = await upload_
    if node.hash != hash_:
        print(f"mismatch {src}")
        await drive.delete(node)
        raise Exception("upload failed")
    print(f"upload {node.name}")


async def _maybe_upload(drive: Drive, src: Path, dst: Node) -> Node:
    try:
        node = await drive.get_child_by_name(src.name, dst)
    except NodeNotFoundError:
        type_, _ext = guess_type(src)
        node = await upload_file_from_local(drive, src, dst, mime_type=type_)
    return node
