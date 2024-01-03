import random
import string
from contextlib import contextmanager
from datetime import datetime, UTC
from pathlib import Path
from tempfile import TemporaryDirectory

from wcpan.drive.core.types import Node


_STR_LIST = string.digits + string.ascii_letters


def random_bool() -> bool:
    return random.choice((True, False))


def random_int() -> int:
    return random.randint(0, 1024)


def random_date() -> datetime:
    return datetime.now(UTC).replace(microsecond=0)


def random_str() -> str:
    return "".join(random.choices(_STR_LIST, k=64))


def random_private() -> dict[str, str]:
    keys = random.randint(0, 4)
    pairs = ((random_str(), random_str()) for _ in range(keys))
    return {k: v for k, v in pairs}


def random_root() -> Node:
    return Node(
        id=random_str(),
        parent_id=None,
        name="",
        is_directory=True,
        is_trashed=False,
        ctime=random_date(),
        mtime=random_date(),
        mime_type="",
        hash="",
        size=0,
        is_image=False,
        is_video=False,
        width=0,
        height=0,
        ms_duration=0,
        private=random_private(),
    )


def random_dir(parent_id: str) -> Node:
    return Node(
        id=random_str(),
        parent_id=parent_id,
        name=random_str(),
        is_directory=True,
        is_trashed=random_bool(),
        ctime=random_date(),
        mtime=random_date(),
        mime_type="",
        hash="",
        size=0,
        is_image=False,
        is_video=False,
        width=0,
        height=0,
        ms_duration=0,
        private=random_private(),
    )


def random_file(parent_id: str) -> Node:
    return Node(
        id=random_str(),
        parent_id=parent_id,
        name=random_str(),
        is_directory=False,
        is_trashed=random_bool(),
        ctime=random_date(),
        mtime=random_date(),
        mime_type="application/octet-stream",
        hash=random_str(),
        size=random_int(),
        is_image=False,
        is_video=False,
        width=0,
        height=0,
        ms_duration=0,
        private=random_private(),
    )


def random_image(parent_id: str):
    return Node(
        id=random_str(),
        parent_id=parent_id,
        name=random_str(),
        is_directory=False,
        is_trashed=random_bool(),
        ctime=random_date(),
        mtime=random_date(),
        mime_type="image/svg+xml",
        hash=random_str(),
        size=random_int(),
        is_image=True,
        is_video=False,
        width=random_int(),
        height=random_int(),
        ms_duration=0,
        private=random_private(),
    )


def random_video(parent_id: str) -> Node:
    return Node(
        id=random_str(),
        parent_id=parent_id,
        name=random_str(),
        is_directory=False,
        is_trashed=random_bool(),
        ctime=random_date(),
        mtime=random_date(),
        mime_type="video/mp4",
        hash=random_str(),
        size=random_int(),
        is_image=False,
        is_video=True,
        width=random_int(),
        height=random_int(),
        ms_duration=random_int(),
        private=random_private(),
    )


@contextmanager
def sandbox():
    with TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        data_path = tmp_path / "sandbox.sqlite"
        dsn = str(data_path)
        # initialize(dsn)
        # root = random_root()
        # set_root(dsn, root)
        # with read_write(dsn) as query:
        #     for _ in range(10000):
        #         node = random_image(root.id)
        #         inner_insert_node(query, node)
        yield dsn
