from asyncio import TaskGroup
from concurrent.futures import Executor
from pathlib import Path
from mimetypes import guess_type

from wcpan.drive.core.types import Drive, Node
from wcpan.drive.core.lib import upload_file_from_local

from ._check import get_hash


async def upload(drive: Drive, src: Path, dst: Node, pool: Executor) -> None:
    type_, _ext = guess_type(src)
    async with TaskGroup() as group:
        factory = await drive.get_hasher_factory()
        check = group.create_task(get_hash(src, factory, pool))
        upload_ = group.create_task(upload_file_from_local(drive, src, dst, mime_type=type_))
        hash_ = await check
        node = await upload_
    assert node.hash == hash_
    print(f"upload {node.name}")
