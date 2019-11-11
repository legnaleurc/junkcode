import os
import os.path as op
from typing import Text
import unittest.mock as utm


def create_entry(path: Text, is_dir: bool) -> os.DirEntry:
    mock = utm.MagicMock(spec=os.DirEntry)
    mock.is_dir = utm.Mock(return_value=is_dir)
    mock.path = path
    mock.name = op.basename(path)
    return mock


def create_dir_entry(path: Text) -> os.DirEntry:
    return create_entry(path, is_dir=True)


def create_file_entry(path: Text) -> os.DirEntry:
    return create_entry(path, is_dir=False)
