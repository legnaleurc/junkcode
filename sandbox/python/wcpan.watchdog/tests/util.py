import contextlib
import os
import os.path as op
import pathlib
from typing import Text, Iterator
import unittest.mock as utm


def create_entry(path: Text, is_dir: bool) -> os.DirEntry:
    mock = utm.Mock(spec=os.DirEntry)
    mock.is_dir = utm.Mock(return_value=is_dir)
    mock.path = path
    mock.name = op.basename(path)
    return mock


@contextlib.contextmanager
def create_dir_entry(path: Text) -> Iterator[os.DirEntry]:
    with utm.patch('pathlib.Path') as FakePath:
        fake_path = FakePath.return_value
        fake_path.parts = pathlib.PurePath(path).parts
        fake_path.is_dir.return_value = True
        yield create_entry(path, is_dir=True)


@contextlib.contextmanager
def create_file_entry(path: Text) -> Iterator[os.DirEntry]:
    with utm.patch('pathlib.Path') as FakePath:
        fake_path = FakePath.return_value
        fake_path.parts = pathlib.PurePath(path).parts
        fake_path.is_dir.return_value = False
        yield create_entry(path, is_dir=False)
