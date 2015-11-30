#! /usr/bin/env python3
# ex: et sts=4 ts=4 sw=4

import sys
import os
import shutil

from acdcli.api import client as ACD
from acdcli.cache import db as DB
from acdcli.api.common import RequestError
from acdcli.utils import hashing
from acdcli.utils.time import datetime_to_timestamp


class Cache(object):

    def __init__(self, cache_folder):
        self._cache_folder = cache_folder
        auth_folder = os.path.expanduser('~/.cache/acd_cli')
        self._acd_client = ACD.ACDClient(auth_folder)
        self._acd_db = DB.NodeCache(auth_folder)
        self._last_recycle = 0

    def __call__(self):
        folder_id = self._acd_db.resolve_path('/tmp')
        children = self._acd_db.list_children(folder_id)
        children = map(lambda _: _.node, children)
        children = sorted(children, key=lambda _: _.modified, reverse=True)
        for child in children:
            self._recycle_space()
            if self._is_too_old(child):
                break
            self._download(child, self._cache_folder)

    def _recycle_space(self):
        entries = self._get_cache_entries()

        while True:
            free_space = self._get_free_space()
            if free_space > 10:
                break

            full_path, mtime = entries.pop(0)
            if os.path.isdir(full_path):
                shutil.rmtree(full_path)
            else:
                os.remove(full_path)
            self._last_recycle = mtime
            print('recycled: ' + full_path)

    def _get_cache_entries(self):
        entries = os.listdir(self._cache_folder)
        entries = (os.path.join(self._cache_folder, _) for _ in entries)
        entries = ((_, os.stat(_).st_mtime) for _ in entries)
        entries = sorted(entries, key=lambda _: _[1])
        return entries

    def _get_free_space(self):
        s = os.statvfs(self._cache_folder)
        s = s.f_frsize * s.f_bavail
        s = s / 1024
        s = s / 1024 / 1024
        return s

    def _is_too_old(self, node):
        return datetime_to_timestamp(node.modified) < self._last_recycle

    def _download(self, node, local_path):
        local_path = local_path if local_path else ''
        full_path = os.path.join(local_path, node.name)

        if not node.is_available():
            return False

        if node.is_folder():
            try:
                os.makedirs(full_path, exist_ok=True)
            except OSError:
                print('mkdir failed: ' + full_path)
                return False
            for child in node.children:
                self._download(child, full_path)
        else:
            if os.path.isfile(full_path):
                print('skip existed: ' + full_path)
                if os.path.getsize(full_path) != node.size:
                    print('size mismatch: ' + full_path)
                    return False
                return True

            hasher = hashing.IncrementalHasher()
            try:
                print('downloading: ' + full_path)
                self._acd_client.download_file(node.id, node.name, local_path, write_callbacks=[hasher.update])
                print('downloaded: ' + full_path)
            except RequestError as e:
                print('download failed: ' + e)
                return False
            else:
                local = hasher.get_result()
                remote = node.md5
                if local != remote:
                    print('md5 mismatch: ' + full_path)
                    return False

        preserve_mtime(node, full_path)

        return True


def main(args=None):
    if args is None:
        args = sys.argv

    cache = Cache(args[1])
    cache()

    return 0


def preserve_mtime(node, full_path):
    mtime = datetime_to_timestamp(node.modified)
    os.utime(full_path, (mtime, mtime))


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
