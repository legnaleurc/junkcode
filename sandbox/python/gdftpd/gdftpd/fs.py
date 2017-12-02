import asyncio
import os.path as op
import pathlib as pl
import stat

import aioftp
import tornado.platform.asyncio as tpa


class GoogleDrivePathIO(aioftp.AbstractPathIO):

    def __init__(self, drive, timeout=None, loop=None):
        super().__init__(timeout, loop)

        self._drive = drive

    async def exists(self, path):
        path = abs_path(path)
        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f
        return node is not None

    async def is_dir(self, path):
        path = abs_path(path)
        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f
        return node.is_folder

    async def is_file(self, path):
        return not await self.is_dir(path)

    def list(self, path):
        return GoogleDriveLister(self._drive, path, self.timeout, self.loop)

    async def stat(self, path):
        s = GoogleDriveStat(self._drive, path)
        await s.initialize()
        return s


class GoogleDriveLister(aioftp.AbstractAsyncLister):

    def __init__(self, drive, path, timeout=None, loop=None):
        super().__init__(timeout=timeout, loop=loop)

        self._drive = drive
        self._path = path
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

        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f

        f = self._drive.get_children(node)
        f = asyncio.wrap_future(f)
        children = await f

        children = (pl.Path(path, _.name) for _ in children)
        return children


class GoogleDriveStat(object):

    def __init__(self, drive, path):
        self._drive = drive
        self._path = path

    async def initialize(self):
        path = abs_path(self._path)

        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f

        if not node:
            raise FileNotFoundError

        if node.is_file:
            self.st_size = node.size
            self.st_mode = stat.S_IFREG | 0o444
        else:
            self.st_size = 0
            self.st_mode = stat.S_IFDIR | 0o555

        self.st_mtime = node.modified.timestamp()
        self.st_ctime = node.created.timestamp()
        self.st_nlink = 1


def abs_path(path):
    if op.isabs(path):
        return path
    path = op.join('/', path)
    path = op.normpath(path)
    return path
