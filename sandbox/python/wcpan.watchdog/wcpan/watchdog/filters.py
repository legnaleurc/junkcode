import enum
import functools
import os
import re
from typing import Dict, Pattern, Set, Tuple, Union


def exclude(fn):
    @functools.wraps(fn)
    def wrapper(entry: os.DirEntry, *args, **kwargs) -> bool:
        return not fn(entry, *args, **kwargs)
    return wrapper


def file_only(fn):
    @functools.wraps(fn)
    def wrapper(entry: os.DirEntry, *args, **kwargs):
        if entry.is_dir():
            return False
        return fn(entry, *args, **kwargs)
    return wrapper


def folder_only(fn):
    @functools.wraps(fn)
    def wrapper(entry: os.DirEntry, *args, **kwargs):
        if not entry.is_dir():
            return False
        return fn(entry, *args, **kwargs)
    return wrapper


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


def is_python_cache(entry: os.DirEntry) -> bool:
    if entry.is_dir():
        return entry.name in ('__pycache__', 'site-packages')
    else:
        return re.search(r'\.py[cod]$', entry.name) is not None


def is_editor_file(entry: os.DirEntry) -> bool:
    FOLDER_PATTERNS = ('.idea', '.vscode')
    FILE_PATTERNS = (r'\.sw.$', r'~$')

    if entry.is_dir():
        g = (_ == entry.name for _ in FOLDER_PATTERNS)
    else:
        g = (re.search(_, entry.name) for _ in FILE_PATTERNS)
    return any(g)


@folder_only
def is_vcs_folder(entry: os.DirEntry) -> bool:
    PATTERNS = ('.git', '.hg', '.svn', '.cvs')
    g = (_ == entry.name for _ in PATTERNS)
    return any(g)


@folder_only
def is_nodejs_cache(entry: os.DirEntry) -> bool:
    return entry.name == 'node_modules'


class PythonFilter(DefaultDirFilter):

    def should_watch_file(self, entry: os.DirEntry) -> bool:
        return entry.name.endswith(('.py', '.pyx', '.pyd'))


def is_python_file(entry: os.DirEntry) -> bool:
    return not entry.is_dir() and entry.name.endswith(('.py', '.pyx', '.pyd'))


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
