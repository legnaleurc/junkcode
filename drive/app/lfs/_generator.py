import shlex
import sys
from pathlib import Path

import yaml

from ._types import MediaDescriptor, AudioStream


H264_PRESET = "veryslow"
H264_CRF = "18"
MP4_FLAGS = "+faststart"


async def generate() -> None:
    data_list: list[MediaDescriptor] = yaml.safe_load(sys.stdin)
    for data in data_list:
        path = data["path"]
        meta = data["meta"]

        main_cmd = ["ffmpeg", "-nostdin", "-y"]
        src_cmd = ["-i", path]
        dst_cmd = _get_dst_cmd(path)
        video_cmd = _get_video_cmd(meta["is_h264"])
        audio_cmd = _get_audio_cmd(meta["audios"])
        subtitle_cmd = ["-map", "-0:s"]
        data_cmd = ["-map", "-0:d"]

        cmd = (
            main_cmd
            + src_cmd
            + video_cmd
            + audio_cmd
            + subtitle_cmd
            + data_cmd
            + dst_cmd
        )
        print(shlex.join(cmd))


def _get_video_cmd(is_h264: bool) -> list[str]:
    mapping = ["-map", "0:v:0"]
    if is_h264:
        return mapping + ["-c:v", "copy"]
    else:
        return mapping + ["-c:v", "libx264", "-crf", H264_CRF, "-preset", H264_PRESET]


def _get_audio_cmd(audios: list[AudioStream]) -> list[str]:
    rv: list[str] = []
    for audio in audios:
        index = audio["index"]
        is_aac = audio["is_aac"]
        mapping = ["-map", f"0:a:{index}"]
        if is_aac:
            rv += mapping + ["-c:a", "copy"]
        else:
            rv += mapping
    return rv


def _get_dst_cmd(src: str) -> list[str]:
    dst = str(Path(src).with_suffix(".mp4"))
    opts = ["-movflags", MP4_FLAGS]
    return opts + [dst]
