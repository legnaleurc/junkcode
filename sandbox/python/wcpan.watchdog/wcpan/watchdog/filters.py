import fnmatch
import functools
import os
import re


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


def is_python_file(entry: os.DirEntry) -> bool:
    return not entry.is_dir() and entry.name.endswith(('.py', '.pyx', '.pyd'))


def matches_glob(entry: os.DirEntry, pattern: str) -> bool:
    return fnmatch.fnmatch(entry.name, pattern)


def matches_regex(entry: os.DirEntry, pattern: str) -> bool:
    return re.search(pattern, entry.name) is not None
