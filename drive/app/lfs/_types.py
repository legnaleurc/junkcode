from typing import TypedDict


class AudioStream(TypedDict):
    index: int
    is_aac: bool
    tags: str


class MediaContainer(TypedDict):
    is_h264: bool
    audios: list[AudioStream]


class MediaDescriptor(TypedDict):
    path: str
    meta: MediaContainer
