from pathlib import Path
from mimetypes import guess_type

from wcpan.drive.core.types import Drive, Node
from wcpan.drive.core.lib import upload_file_from_local


async def upload(drive: Drive, src: Path, dst: Node) -> None:
    type_, _ext = guess_type(src)
    print(f"upload {src}")
    await upload_file_from_local(drive, src, dst, mime_type=type_)
    print(f"upload ok")
