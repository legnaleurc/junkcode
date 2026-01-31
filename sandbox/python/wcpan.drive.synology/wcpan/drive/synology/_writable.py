"""
WritableFile implementation for uploading files to Synology.
"""

from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from io import BytesIO
from typing import override

from wcpan.drive.core.types import MediaInfo, Node, PrivateDict, WritableFile

from ._lib import node_from_api
from ._network import Network


@asynccontextmanager
async def create_writable(
    *,
    network: Network,
    parent_path: str,
    name: str,
    size: int | None = None,
    mime_type: str | None = None,
    media_info: MediaInfo | None = None,
    private: PrivateDict | None = None,
) -> AsyncIterator[WritableFile]:
    """
    Create a writable file for uploading to Synology.

    Args:
        network: Network instance
        parent_path: Parent folder path
        name: File name
        size: File size (optional, not used for now)
        mime_type: MIME type (optional)
        media_info: Media metadata (optional, not used for now)
        private: Private data (not supported by Synology)

    Yields:
        WritableFile instance
    """
    # Handle empty files
    if size is not None and size <= 0:
        yield await create_empty(
            network=network,
            parent_path=parent_path,
            name=name,
            mime_type=mime_type,
        )
        return

    # For non-empty files, use buffered upload
    async with create_buffered(
        network=network,
        parent_path=parent_path,
        name=name,
        size=size,
        mime_type=mime_type,
    ) as fout:
        yield fout


async def create_empty(
    *,
    network: Network,
    parent_path: str,
    name: str,
    mime_type: str | None = None,
) -> WritableFile:
    """
    Create an empty file on Synology.

    Args:
        network: Network instance
        parent_path: Parent folder path
        name: File name
        mime_type: MIME type (optional)

    Returns:
        WritableFile for empty file
    """
    from ._api import files

    # Upload empty content
    async def empty_generator() -> AsyncIterator[bytes]:
        return
        yield  # Make it an async generator

    file_dict = await files.upload_file(
        network=network,
        parent_path=parent_path,
        name=name,
        data=empty_generator(),
        size=0,
        mime_type=mime_type,
    )

    node = node_from_api(file_dict)
    return SynologyEmptyWritableFile(node)


class SynologyEmptyWritableFile(WritableFile):
    """
    WritableFile for empty files.
    All write operations are no-ops.
    """

    def __init__(self, node: Node) -> None:
        self._node = node

    @override
    async def write(self, chunk: bytes) -> int:
        """Write does nothing for empty files."""
        return 0

    @override
    async def seek(self, offset: int) -> int:
        """Seek does nothing for empty files."""
        return 0

    @override
    async def tell(self) -> int:
        """Always at position 0."""
        return 0

    @override
    async def flush(self) -> None:
        """Flush does nothing for empty files."""
        pass

    @override
    async def node(self) -> Node:
        """Get the node."""
        return self._node


@asynccontextmanager
async def create_buffered(
    *,
    network: Network,
    parent_path: str,
    name: str,
    size: int | None = None,
    mime_type: str | None = None,
) -> AsyncIterator[WritableFile]:
    """
    Create a buffered writable file.

    This implementation buffers the file content in memory and uploads
    on flush. For large files, this might not be optimal, but it's simple
    and works for most cases.

    Args:
        network: Network instance
        parent_path: Parent folder path
        name: File name
        size: Expected file size (optional)
        mime_type: MIME type (optional)

    Yields:
        WritableFile instance
    """
    buffer = BytesIO()
    writable = SynologyBufferedWritableFile(
        network=network,
        parent_path=parent_path,
        name=name,
        buffer=buffer,
        mime_type=mime_type,
    )

    yield writable

    # Ensure flush happens on exit
    await writable.flush()


class SynologyBufferedWritableFile(WritableFile):
    """
    WritableFile that buffers content in memory.

    This is a simple implementation that collects all writes in a BytesIO
    buffer and uploads the entire file on flush.
    """

    def __init__(
        self,
        *,
        network: Network,
        parent_path: str,
        name: str,
        buffer: BytesIO,
        mime_type: str | None = None,
    ) -> None:
        self._network = network
        self._parent_path = parent_path
        self._name = name
        self._buffer = buffer
        self._mime_type = mime_type
        self._node: Node | None = None
        self._flushed = False

    @override
    async def write(self, chunk: bytes) -> int:
        """
        Write chunk to buffer.

        Args:
            chunk: Bytes to write

        Returns:
            Number of bytes written
        """
        return self._buffer.write(chunk)

    @override
    async def seek(self, offset: int) -> int:
        """
        Seek to position in buffer.

        Args:
            offset: Byte offset from start

        Returns:
            New position
        """
        return self._buffer.seek(offset)

    @override
    async def tell(self) -> int:
        """
        Get current position in buffer.

        Returns:
            Current byte position
        """
        return self._buffer.tell()

    @override
    async def flush(self) -> None:
        """
        Flush buffer and upload file to Synology.
        """
        if self._flushed:
            return

        from ._api import files

        # Get buffer content
        content = self._buffer.getvalue()
        size = len(content)

        # Create async generator from buffer
        async def content_generator() -> AsyncIterator[bytes]:
            yield content

        # Upload file
        file_dict = await files.upload_file(
            network=self._network,
            parent_path=self._parent_path,
            name=self._name,
            data=content_generator(),
            size=size,
            mime_type=self._mime_type,
        )

        # Convert to node
        self._node = node_from_api(file_dict)
        self._flushed = True

    @override
    async def node(self) -> Node:
        """
        Get the uploaded node.

        Returns:
            Node object

        Raises:
            Exception: If file hasn't been flushed yet
        """
        if not self._flushed:
            await self.flush()

        if not self._node:
            raise Exception("Upload failed: no node created")

        return self._node
