"""
ReadableFile implementation for downloading files from Synology.
"""

from collections.abc import AsyncIterator
from contextlib import AsyncExitStack, asynccontextmanager
from functools import partial
from logging import getLogger
from typing import Any, override

from aiohttp import ClientResponse

from wcpan.drive.core.types import Node, ReadableFile

from ._network import Network


class SynologyReadableFile(ReadableFile):
    """
    ReadableFile implementation for Synology Drive.
    Supports streaming download with range requests for seeking.
    """

    def __init__(self, network: Network, node: Node) -> None:
        """
        Initialize ReadableFile.

        Args:
            network: Network instance
            node: Node to read
        """
        self._network = network
        self._node = node
        self._offset = 0
        self._response: ClientResponse | None = None
        self._rsps: AsyncExitStack | None = None

    async def __aenter__(self) -> ReadableFile:
        """Enter async context."""
        return self

    async def __aexit__(self, type_: Any, exc: Any, tb: Any) -> None:
        """Exit async context and cleanup."""
        await self._close_response()

    @override
    async def __aiter__(self) -> AsyncIterator[bytes]:
        """
        Iterate over file content as chunks.

        Yields:
            Byte chunks
        """
        # Empty file has nothing to iterate
        if self._node.size <= 0:
            return

        await self._open_response()
        assert self._response
        async for chunk in self._response.content.iter_any():
            yield chunk

    @override
    async def read(self, length: int) -> bytes:
        """
        Read up to length bytes from the file.

        Args:
            length: Maximum number of bytes to read

        Returns:
            Bytes read
        """
        # Empty file returns empty bytes
        if self._node.size <= 0:
            return b""

        await self._open_response()
        assert self._response
        chunk = await self._response.content.read(length)
        self._offset += len(chunk)
        return chunk

    @override
    async def seek(self, offset: int) -> int:
        """
        Seek to a specific position in the file.

        Args:
            offset: Byte offset from start

        Returns:
            New offset position
        """
        # Empty file always at offset 0
        if self._node.size <= 0:
            return 0

        # Close current response and open new one at offset
        self._offset = offset
        await self._close_response()
        await self._open_response()
        return self._offset

    @override
    async def node(self) -> Node:
        """
        Get the node being read.

        Returns:
            Node object
        """
        return self._node

    @asynccontextmanager
    async def _download_from_offset(self) -> AsyncIterator[ClientResponse]:
        """
        Download file starting from current offset.

        Yields:
            ClientResponse for streaming
        """
        from ._api import files

        # Calculate range
        range_start = self._offset
        range_end = self._node.size - 1

        # Use file path (ID) to download
        file_path = self._node.id

        getLogger(__name__).debug(
            f"Downloading {file_path} from offset {range_start} to {range_end}"
        )

        dl = partial(
            files.download,
            self._network,
            file_path,
            (range_start, range_end),
        )

        async with dl() as response:
            async for resp in response:
                yield resp

    async def _open_response(self) -> None:
        """Open download response if not already open."""
        if not self._response:
            async with AsyncExitStack() as stack:
                self._response = await stack.enter_async_context(
                    self._download_from_offset()
                )
                self._rsps = stack.pop_all()

    async def _close_response(self) -> None:
        """Close download response if open."""
        if self._response:
            assert self._rsps
            await self._rsps.aclose()
            self._response = None
            self._rsps = None
