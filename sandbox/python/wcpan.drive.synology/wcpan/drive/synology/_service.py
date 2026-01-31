"""
Main FileService implementation for Synology Drive.
"""

from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from logging import getLogger
from typing import override

from aiohttp import ClientSession

from wcpan.drive.core.exceptions import NodeExistsError
from wcpan.drive.core.types import (
    ChangeAction,
    CreateHasher,
    FileService,
    MediaInfo,
    Node,
    PrivateDict,
    ReadableFile,
    WritableFile,
)

from ._auth import AuthManager
from ._hasher import create_hasher
from ._lib import node_from_api, normalize_changes
from ._network import Network
from ._readable import SynologyReadableFile
from ._writable import create_writable


@asynccontextmanager
async def create_file_service(
    username: str,
    password: str,
    base_url: str,
    otp_code: str | None = None,
) -> AsyncIterator[FileService]:
    """
    Create a Synology Drive file service.

    Args:
        username: Synology account username
        password: Synology account password
        base_url: Synology NAS base URL (e.g., "https://nas.example.com:5001")
        otp_code: Optional OTP code for 2FA

    Yields:
        FileService instance
    """
    auth = AuthManager(username, password, base_url, otp_code)

    async with ClientSession() as session:
        # Authenticate
        await auth.ensure_authenticated(session)

        # Create network layer
        network = Network(session, auth)

        # Yield service
        service = SynologyDriveFileService(network, auth)
        try:
            yield service
        finally:
            # Logout on cleanup
            await auth.logout(session)


class SynologyDriveFileService(FileService):
    """
    FileService implementation for Synology Drive.
    """

    def __init__(self, network: Network, auth: AuthManager) -> None:
        """
        Initialize the service.

        Args:
            network: Network instance
            auth: Authentication manager
        """
        self._network = network
        self._auth = auth

    @property
    @override
    def api_version(self) -> int:
        """Get API version (always 4 for compatibility)."""
        return 4

    @override
    async def get_initial_cursor(self) -> str:
        """Get the initial cursor for change tracking."""
        return "0"

    @override
    async def get_root(self) -> Node:
        """
        Get the root node.

        Returns:
            Root node
        """
        from ._api import listing

        file_dict = await listing.get_root(self._network)
        return node_from_api(file_dict)

    @override
    async def get_changes(
        self,
        cursor: str,
    ) -> AsyncIterator[tuple[list[ChangeAction], str]]:
        """
        Get file changes since cursor.

        Since Synology doesn't have a change log API, we fetch all files
        and treat them as updates.

        Args:
            cursor: Current cursor (incremental counter)

        Yields:
            Tuple of (change list, new cursor)
        """
        from ._api import listing

        getLogger(__name__).debug(f"Fetching changes from cursor {cursor}")

        # Fetch all files recursively
        async for file_list in listing.list_all_files(self._network):
            # Convert to changes (all updates)
            changes = list(normalize_changes(file_list))

            if changes:
                # Increment cursor
                new_cursor = str(int(cursor) + 1)
                yield changes, new_cursor
                cursor = new_cursor

        # Final cursor update even if no changes
        final_cursor = str(int(cursor) + 1)
        yield [], final_cursor

    @override
    async def move(
        self,
        node: Node,
        *,
        new_parent: Node | None,
        new_name: str | None,
        trashed: bool | None,
    ) -> Node:
        """
        Move/rename a node or change its trash status.

        Args:
            node: Node to modify
            new_parent: New parent node (optional)
            new_name: New name (optional)
            trashed: Trash status (optional, not fully supported)

        Returns:
            Updated node

        Raises:
            ValueError: If trying to move root node
        """
        from ._api import files

        if not node.parent_id:
            raise ValueError("Cannot move root node")

        # Determine new parent path
        new_parent_path = None
        if new_parent:
            new_parent_path = new_parent.id  # ID is path in Synology

        # Update file
        updated_dict = await files.update_file(
            self._network,
            file_path=node.id,
            new_name=new_name,
            new_parent_path=new_parent_path,
            trashed=trashed,
        )

        return node_from_api(updated_dict)

    @override
    async def delete(self, node: Node) -> None:
        """
        Permanently delete a node.

        Args:
            node: Node to delete
        """
        from ._api import files

        await files.delete_file(self._network, node.id, permanent=True)

    @override
    async def purge_trash(self) -> None:
        """
        Purge all trashed items.

        Note: Synology doesn't have a standard trash API,
        so this is a no-op for now.
        """
        getLogger(__name__).warning("Purge trash not implemented for Synology")
        # Synology doesn't have a standard trash purge API
        # Would need to find and delete items in #recycle folder
        pass

    @override
    async def create_directory(
        self,
        name: str,
        parent: Node,
        *,
        exist_ok: bool,
        private: PrivateDict | None,
    ) -> Node:
        """
        Create a directory.

        Args:
            name: Directory name
            parent: Parent node
            exist_ok: Whether to allow existing directory
            private: Private metadata (not supported)

        Returns:
            Created or existing node

        Raises:
            NodeExistsError: If directory exists and exist_ok is False
        """
        from ._api import folders

        # Check if already exists
        try:
            parent_path = parent.id
            folder_path = f"{parent_path.rstrip('/')}/{name}"
            existing = await folders.get_folder_info(self._network, folder_path)

            if exist_ok:
                getLogger(__name__).info(f"Skipped (existing) {name}")
                return node_from_api(existing)
            else:
                raise NodeExistsError(node_from_api(existing))

        except Exception:
            # Folder doesn't exist, create it
            pass

        # Create folder
        folder_dict = await folders.create_folder(
            self._network,
            name=name,
            parent_path=parent.id,
        )

        return node_from_api(folder_dict)

    @asynccontextmanager
    @override
    async def download_file(self, node: Node) -> AsyncIterator[ReadableFile]:
        """
        Download a file.

        Args:
            node: Node to download

        Yields:
            ReadableFile instance
        """
        async with SynologyReadableFile(self._network, node) as fin:
            yield fin

    @asynccontextmanager
    @override
    async def upload_file(
        self,
        name: str,
        parent: Node,
        *,
        size: int | None,
        mime_type: str | None,
        media_info: MediaInfo | None,
        private: PrivateDict | None,
    ) -> AsyncIterator[WritableFile]:
        """
        Upload a file.

        Args:
            name: File name
            parent: Parent node
            size: File size (optional)
            mime_type: MIME type (optional)
            media_info: Media metadata (optional)
            private: Private metadata (not supported)

        Yields:
            WritableFile instance

        Raises:
            NodeExistsError: If file already exists
        """
        # Check if file already exists
        try:
            parent_path = parent.id
            file_path = f"{parent_path.rstrip('/')}/{name}"

            from ._api import files

            existing = await files.get_file_info(self._network, file_path)
            raise NodeExistsError(node_from_api(existing))

        except Exception as e:
            # File doesn't exist (expected), continue with upload
            if isinstance(e, NodeExistsError):
                raise
            pass

        # Upload file
        async with create_writable(
            network=self._network,
            parent_path=parent.id,
            name=name,
            size=size,
            mime_type=mime_type,
            media_info=media_info,
            private=private,
        ) as fout:
            yield fout

    @override
    async def get_hasher_factory(self) -> CreateHasher:
        """
        Get the hasher factory.

        Returns:
            Hasher factory function
        """
        return create_hasher

    @override
    async def is_authorized(self) -> bool:
        """
        Check if authorized (has session token).

        Returns:
            True if authorized, False otherwise
        """
        return self._auth.is_authenticated()

    @override
    async def get_oauth_url(self) -> str:
        """
        Get OAuth URL.

        Note: Synology uses username/password, not OAuth.
        Returns empty string for compatibility.

        Returns:
            Empty string
        """
        return ""

    @override
    async def set_oauth_token(self, token: str) -> None:
        """
        Set OAuth token.

        Note: Synology uses username/password, not OAuth.
        This is a no-op for compatibility.

        Args:
            token: OAuth token (ignored)
        """
        pass

    @property
    def network(self) -> Network:
        """Get the network instance."""
        return self._network
