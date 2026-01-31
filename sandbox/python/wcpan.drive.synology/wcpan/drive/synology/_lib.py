from collections.abc import Callable, Iterable
from concurrent.futures import Executor
from datetime import UTC, datetime
from typing import Concatenate, NotRequired, TypedDict

from wcpan.drive.core.types import ChangeAction, Node


FOLDER_MIME_TYPE = "application/x-folder"


class SynologyFileDict(TypedDict):
    """
    TypedDict representing a file/folder from Synology Drive API.
    """

    id: str
    name: str
    parent_id: NotRequired[str | None]
    is_folder: NotRequired[bool]
    trashed: NotRequired[bool]
    created_time: str  # ISO 8601 format
    modified_time: str  # ISO 8601 format
    mime_type: NotRequired[str]
    size: NotRequired[int]
    hash: NotRequired[str]
    # Media metadata
    image_width: NotRequired[int]
    image_height: NotRequired[int]
    video_width: NotRequired[int]
    video_height: NotRequired[int]
    video_duration_ms: NotRequired[int]


def node_from_api(data: SynologyFileDict) -> Node:
    """
    Convert a Synology API file dict to a Node object.
    """
    id_ = data["id"]
    name = data["name"]
    parent_id = data.get("parent_id")

    # Determine if it's a folder
    is_folder = data.get("is_folder", False)
    if not is_folder and "mime_type" in data:
        is_folder = data["mime_type"] == FOLDER_MIME_TYPE

    # Parse timestamps (Synology uses ISO 8601)
    ctime = datetime.fromisoformat(data["created_time"])
    mtime = datetime.fromisoformat(data["modified_time"])

    # Ensure datetime objects have UTC timezone
    if ctime.tzinfo is None:
        ctime = ctime.replace(tzinfo=UTC)
    if mtime.tzinfo is None:
        mtime = mtime.replace(tzinfo=UTC)

    # File properties
    size = data.get("size", 0)
    hash_ = data.get("hash", "")
    mime_type = "" if is_folder else data.get("mime_type", "")
    is_trashed = data.get("trashed", False)

    # Media metadata
    is_image = False
    is_video = False
    width = 0
    height = 0
    ms_duration = 0

    # Check for image metadata
    if "image_width" in data and "image_height" in data:
        is_image = True
        width = data["image_width"]
        height = data["image_height"]

    # Check for video metadata (overrides image if both present)
    if "video_width" in data and "video_height" in data:
        is_video = True
        is_image = False  # Video takes precedence
        width = data["video_width"]
        height = data["video_height"]
        ms_duration = data.get("video_duration_ms", 0)

    return Node(
        id=id_,
        name=name,
        is_trashed=is_trashed,
        ctime=ctime,
        mtime=mtime,
        parent_id=parent_id,
        is_directory=is_folder,
        mime_type=mime_type,
        hash=hash_,
        size=size,
        is_image=is_image,
        is_video=is_video,
        width=width,
        height=height,
        ms_duration=ms_duration,
        private=None,  # Synology doesn't have app properties
    )


def normalize_changes(
    file_list: list[SynologyFileDict],
) -> Iterable[ChangeAction]:
    """
    Convert a list of Synology files to ChangeAction updates.
    Since Synology doesn't have a change log, we treat all files as updates.
    """
    for file_data in file_list:
        node = node_from_api(file_data)
        yield False, node


def utc_now() -> datetime:
    """Get current UTC datetime."""
    return datetime.now(UTC)


class OffMainProcess:
    """
    Helper class to run synchronous database functions in a ProcessPoolExecutor.

    This avoids GIL contention by running SQLite operations in separate processes.
    """

    def __init__(self, *, db_path: str, pool: Executor) -> None:
        """
        Initialize the off-main-process executor.

        Args:
            db_path: Path to the database file
            pool: ProcessPoolExecutor instance
        """
        self._db_path = db_path
        self._pool = pool

    async def __call__[
        **A, R
    ](
        self, fn: Callable[Concatenate[str, A], R], *args: A.args, **kwargs: A.kwargs
    ) -> R:
        """
        Execute a function in the process pool.

        The function will be called with db_path as the first argument,
        followed by any additional args and kwargs.

        Args:
            fn: Function to execute (must accept db_path as first param)
            *args: Additional positional arguments
            **kwargs: Additional keyword arguments

        Returns:
            Function result
        """
        from asyncio import get_running_loop
        from functools import partial

        bound = partial(fn, self._db_path, *args, **kwargs)
        loop = get_running_loop()
        return await loop.run_in_executor(self._pool, bound)
