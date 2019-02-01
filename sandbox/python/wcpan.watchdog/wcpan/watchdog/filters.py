import enum
import os
import re
from typing import Dict, Pattern, Set, Tuple, Union


__all__ = ('Change', 'AllFilter', 'DefaultDirFilter', 'DefaultFilter',
           'PythonFilter', 'RegExpFilter')


class Change(enum.IntEnum):

    added = 1
    modified = 2
    deleted = 3


ChangeEntry = Tuple[Change, str]
ChangeSet = Set[ChangeEntry]
Snapshot = Dict[str, int]


class AllFilter(object):

    def __init__(self, root_path: str):
        self.files = {}
        self.root_path = root_path

    def should_watch_dir(self, entry: os.DirEntry) -> bool:
        return True

    def should_watch_file(self, entry: os.DirEntry) -> bool:
        return True

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


class DefaultDirFilter(AllFilter):

    ignored_dirs = {'.git', '__pycache__', 'site-packages', '.idea',
                    'node_modules'}

    def should_watch_dir(self, entry: os.DirEntry) -> bool:
        return entry.name not in self.ignored_dirs


class DefaultFilter(DefaultDirFilter):

    ignored_file_regexes = (r'\.py[cod]$', r'\.___jb_...___$', r'\.sw.$', '~$')

    def __init__(self, root_path: str):
        super().__init__(root_path)

        self._ignored_file_regexes = tuple(re.compile(r) for r in self.ignored_file_regexes)

    def should_watch_file(self, entry: os.DirEntry) -> bool:
        return not any(r.search(entry.name) for r in self._ignored_file_regexes)


class PythonFilter(DefaultDirFilter):

    def should_watch_file(self, entry: os.DirEntry) -> bool:
        return entry.name.endswith(('.py', '.pyx', '.pyd'))


class RegExpFilter(AllFilter):

    def __init__(self,
        root_path: str,
        re_files: Union[Pattern, str] = None,
        re_dirs: Union[Pattern, str] = None,
    ):
        super().__init__(root_path)

        self.re_files = re.compile(re_files) if re_files is not None else re_files
        self.re_dirs = re.compile(re_dirs) if re_dirs is not None else re_dirs

    def should_watch_file(self, entry: os.DirEntry) -> bool:
        return self.re_files.match(entry.path)

    def should_watch_dir(self, entry: os.DirEntry) -> bool:
        return self.re_dirs.match(entry.path)
