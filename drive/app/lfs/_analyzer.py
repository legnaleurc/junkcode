import sys
from collections.abc import Iterator
from pathlib import Path
from typing import Any

import magic
import yaml

from ._types import MediaContainer, MediaDescriptor


_VIDEO_CODEC_SET: set[str] = {"h264", "hevc"}


async def analyze(root_path: Path) -> None:
    for file_path in _scan(root_path):
        descriptor = await _describe(file_path)
        meta = _transform(descriptor)

        data: MediaDescriptor = {
            "path": str(file_path),
            "meta": meta,
        }
        yaml.safe_dump(
            [data],
            sys.stdout,
            encoding="utf-8",
            allow_unicode=True,
            default_flow_style=False,
        )


def _scan(root_path: Path) -> Iterator[Path]:
    for root, _, files in root_path.walk():
        for file_ in files:
            file_path = root / file_

            if not _is_video(file_path):
                continue

            yield file_path


def _is_video(file_path: Path):
    mime_type = magic.from_file(file_path, mime=True)  # type: ignore
    return mime_type.startswith("video/")


async def _describe(file_path: Path) -> Any:
    from asyncio import create_subprocess_exec
    from asyncio.subprocess import PIPE
    from json import loads

    cmd = [
        "ffprobe",
        "-v",
        "quiet",
        "-show_streams",
        "-print_format",
        "json",
        "-i",
        str(file_path),
    ]
    p = await create_subprocess_exec(*cmd, stdout=PIPE, stderr=PIPE)
    out, _ = await p.communicate()
    raw_out = out.decode("utf-8", "ignore")
    descriptor = loads(raw_out)
    return descriptor


def _transform(descriptor: dict[str, Any]) -> MediaContainer:
    rv: MediaContainer = {
        "is_h264": False,
        "audios": [],
    }
    streams: list[Any] = descriptor["streams"]
    for stream in streams:
        codec_type: str = stream["codec_type"]
        match codec_type:
            case "video":
                codec_name: str = stream["codec_name"]
                rv["is_h264"] = codec_name in _VIDEO_CODEC_SET
            case "audio":
                codec_name: str = stream["codec_name"]
                rv["audios"].append(
                    {
                        "index": len(rv["audios"]),
                        "is_aac": codec_name == "aac",
                        "tags": stream.get("tags", None),
                    }
                )
            case _:
                continue
    return rv
