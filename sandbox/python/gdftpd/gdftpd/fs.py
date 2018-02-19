import asyncio
import os.path as op
import pathlib as pl
import stat

import aioftp
import tornado.platform.asyncio as tpa


class GoogleDrivePathIO(aioftp.AbstractPathIO):

    def __init__(self, drive, timeout=None, loop=None, connection=None):
        super().__init__(timeout, loop)

        self._drive = drive
        self._cache = NodeCache()

    async def exists(self, path):
        path = abs_path(path)
        node = await self._drive.get_node_by_path(path)
        return node is not None

    async def is_dir(self, path):
        path = abs_path(path)
        node = await self._drive.get_node_by_path(path)
        return node.is_folder

    async def is_file(self, path):
        return not await self.is_dir(path)

    def list(self, path):
        return GoogleDriveLister(self._drive, path, self._cache, self.timeout,
                                 self.loop)

    async def stat(self, path):
        s = GoogleDriveStat(self._drive, path, self._cache)
        await s.initialize()
        return s

    async def rename(self, source, destination):
        source = abs_path(source)
        destination = abs_path(destination)
        await self._drive.rename_node_by_path(source, destination)
        self._cache.delete_path(source)

    async def unlink(self, path):
        path = abs_path(path)
        node = await self._drive.get_node_by_path(path)
        await self._drive.trash_node(node)
        self._cache.delete_path(path)


class GoogleDriveLister(aioftp.AbstractAsyncLister):

    def __init__(self, drive, path, cache, timeout=None, loop=None):
        super().__init__(timeout=timeout, loop=loop)

        self._drive = drive
        self._path = path
        self._cache = cache
        self._children = None

    @aioftp.with_timeout
    async def __aiter__(self):
        return self

    @aioftp.with_timeout
    async def __anext__(self):
        if not self._children:
            self._children = await self._fetch()
        try:
            path = next(self._children)
        except StopIteration:
            raise StopAsyncIteration
        return path

    async def _fetch(self):
        path = abs_path(self._path)

        node = await self._drive.get_node_by_path(path)
        children = await self._drive.get_children(node)

        return iter_node_list(path, children, self._cache)


class GoogleDriveStat(object):

    def __init__(self, drive, path, cache):
        self._drive = drive
        self._path = path
        self._cache = cache

    async def initialize(self):
        path = abs_path(self._path)

        try:
            node = self._cache.get_path(path)
        except KeyError:
            node = await self._drive.get_node_by_path(path)

        if not node:
            raise FileNotFoundError(path)
        self._cache.update_path(path, node)

        if node.is_file:
            self.st_size = node.size
            self.st_mode = stat.S_IFREG | 0o444
        else:
            self.st_size = 0
            self.st_mode = stat.S_IFDIR | 0o555

        self.st_mtime = node.modified.timestamp()
        self.st_ctime = node.created.timestamp()
        self.st_nlink = 1


class NodeCache(object):

    def __init__(self):
        self._cache = {}

    def update_path(self, path, node):
        self._cache[path] = node

    def delete_path(self, path):
        del self._cache[path]

    def get_path(self, path):
        return self._cache[path]


def abs_path(path):
    if op.isabs(path):
        return path
    path = op.join('/', path)
    path = op.normpath(path)
    return path


def iter_node_list(parent_path, node_list, cache):
    for node in node_list:
        path = pl.Path(parent_path, node.name)
        cache.update_path(path, node)
        yield path.relative_to('/')
