import enum
import os
from typing import Dict, Pattern, Set, Tuple, Union


class Change(enum.IntEnum):

    added = 1
    modified = 2
    deleted = 3


ChangeEntry = Tuple[Change, str]
ChangeSet = Set[ChangeEntry]
Snapshot = Dict[str, int]


class Walker(object):

    def __init__(self, root_path: str):
        self.files = {}
        self.root_path = root_path

    def _walk(self, dir_path: str, changes: ChangeSet, new_files: Snapshot):
        for entry in os.scandir(dir_path):
            if entry.is_dir():
                if self.should_watch_dir(entry):
                    self._walk(entry.path, changes, new_files)
            elif self.should_watch_file(entry):
                mtime = entry.stat().st_mtime
                new_files[entry.path] = mtime
                old_mtime = self.files.get(entry.path)
                if not old_mtime:
                    changes.add((Change.added, entry.path))
                elif old_mtime != mtime:
                    changes.add((Change.modified, entry.path))

    def __call__(self) -> ChangeSet:
        changes = set()
        new_files = {}
        try:
            self._walk(str(self.root_path), changes, new_files)
        except OSError as e:
            # happens when a directory has been deleted between checks
            print(f'error walking file system: {e.__class__.__name__} {e}')

        # look for deleted
        deleted = self.files.keys() - new_files.keys()
        if deleted:
            changes |= {(Change.deleted, entry) for entry in deleted}

        self.files = new_files
        return changes

