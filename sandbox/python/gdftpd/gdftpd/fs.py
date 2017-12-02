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
        f = tpa.to_asyncio_future(f)
        node = await f
        return node is not None

    async def list(self, path):
        path = op.join('/', path)
        path = op.normpath(path)

        f = self._drive.get_node_by_path(path)
        f = tpa.to_asyncio_future(f)
        node = await f

        f = self._drive.get_children(node)
        f = tpa.to_asyncio_future(f)
        children = await f

        return GoogleDriveLister(children, self.timeout, self.loop)


class GoogleDriveLister(aioftp.AbstractAsyncLister):

    def __init__(self, children, timeout=None, loop=None):
        super().__init__(timeout, loop)

        self._children = iter(children)

    @aioftp.with_timeout
    def __aiter__(self):
        return self

    @aioftp.with_timeout
    async def __anext__(self):
        try:
            node = next(self._children)
        except StopIteration:
            raise StopAsyncIteration
        print('anext', node)
        return node.name
