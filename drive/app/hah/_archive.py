from asyncio import create_subprocess_exec
from asyncio.subprocess import DEVNULL
from pathlib import Path


async def archive(src: Path, dst: Path) -> Path:
    name = f"{src.name}.7z"
    tmp_path = dst / name
    cmd = [
        "7z",
        "a",
        "-y",
        str(tmp_path),
        "*",
    ]
    p = await create_subprocess_exec(*cmd, cwd=src, stdin=DEVNULL)
    rv = await p.wait()
    assert rv == 0
    assert tmp_path.is_file()
    return tmp_path
