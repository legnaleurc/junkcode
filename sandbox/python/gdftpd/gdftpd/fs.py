import asyncio
import os.path as op

import aioftp
import tornado.platform.asyncio as tpa


class GoogleDrivePathIO(aioftp.AbstractPathIO):

    def __init__(self, drive, timeout=None, loop=None):
        super().__init__(timeout, loop)

        self._drive = drive

    async def exists(self, path):
        path = op.join('/', path)
        path = op.normpath(path)
        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f
        return node is not None

    def list(self, path):
        return GoogleDriveLister(self._drive, path, self.timeout, self.loop)


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
            node = next(self._children)
        except StopIteration:
            raise StopAsyncIteration
        return node.name

    async def _fetch(self):
        path = op.join('/', self._path)
        path = op.normpath(path)

        f = self._drive.get_node_by_path(path)
        f = asyncio.wrap_future(f)
        node = await f

        f = self._drive.get_children(node)
        f = asyncio.wrap_future(f)
        children = await f

        return iter(children)
