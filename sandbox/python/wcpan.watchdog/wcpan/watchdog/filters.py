import pathlib
import functools
import os
import re
from typing import Callable, Text


FilterFunction = Callable[[os.DirEntry], bool]


class Filter(object):

    def __init__(self):
        self._include_list = []
        self._exclude_list = []

    def __call__(self, entry: os.DirEntry) -> bool:
        should_exclude = (_(entry) for _ in self._exclude_list)
        if any(should_exclude):
            return False

        # default accept all entries
        if not self._include_list:
            return True

        should_include = (_(entry) for _ in self._include_list)
        return any(should_include)

    def include(self, fn: FilterFunction) -> None:
        self._include_list.append(fn)

    def exclude(self, fn: FilterFunction) -> None:
        self._exclude_list.append(fn)


def create_default_filter() -> Filter:
    filter_ = Filter()
    filter_.exclude(is_vcs_folder)
    filter_.exclude(is_editor_file)
    filter_.exclude(is_python_cache)
    filter_.exclude(is_nodejs_cache)
    return filter_


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


@file_only
def is_python_file(entry: os.DirEntry) -> bool:
    return entry.name.endswith(('.py', '.pyx', '.pyd'))


def matches_glob(pattern: Text) -> FilterFunction:
    matcher = GlobPathMatcher(pattern)
    return lambda entry: matcher(entry)


def matches_regex(pattern: Text) -> FilterFunction:
    return lambda entry: re.search(pattern, entry.name) is not None


class GlobPathMatcher(object):

    def __init__(self, pattern: Text):
        parts = pathlib.PurePath(pattern).parts
        self._parts = [re.compile(glob_to_regex(_)) for _ in parts]

    def __call__(self, entry: os.DirEntry) -> bool:
        path = pathlib.Path(entry.path)
        for part, pattern in zip(path.parts, self._parts):
            if not re.match(pattern, part):
                return False
        if len(path.parts) < len(self._parts) and not path.is_dir():
            return False
        return True


# TODO Windows
def glob_to_regex(glob: Text) -> Text:
    return glob.replace('*', r'([^/]*)')
