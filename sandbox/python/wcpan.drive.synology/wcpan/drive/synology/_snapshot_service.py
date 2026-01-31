"""
SQLite-based SnapshotService for local caching.
"""

from concurrent.futures import ProcessPoolExecutor
from contextlib import asynccontextmanager
from pathlib import Path, PurePath
from typing import override

from wcpan.drive.core.exceptions import NodeNotFoundError
from wcpan.drive.core.types import ChangeAction, Node, SnapshotService

from . import _db
from ._lib import OffMainProcess


@asynccontextmanager
async def create_snapshot_service(db_path: str | Path):
    """
    Create a snapshot service backed by SQLite.

    Uses ProcessPoolExecutor to run database operations in separate processes,
    completely avoiding Python's GIL.

    Args:
        db_path: Path to SQLite database file

    Yields:
        SnapshotService instance
    """
    # Convert to string for pickling
    if isinstance(db_path, Path):
        db_path_str = str(db_path)
    else:
        db_path_str = db_path

    # Create process pool
    with ProcessPoolExecutor() as pool:
        # Create off-main-process executor
        bg = OffMainProcess(db_path=db_path_str, pool=pool)

        # Initialize database
        await bg(_db.init_db)

        # Create and yield service
        yield SqliteSnapshotService(bg)


class SqliteSnapshotService(SnapshotService):
    """
    SnapshotService implementation using SQLite for local caching.

    All database operations run in a ProcessPoolExecutor to completely
    avoid Python's GIL and maximize performance.
    """

    def __init__(self, bg: OffMainProcess) -> None:
        """
        Initialize the snapshot service.

        Args:
            bg: Off-main-process executor
        """
        self._bg = bg

    @property
    @override
    def api_version(self) -> int:
        """Get API version (always 4 for compatibility)."""
        return 4

    @override
    async def get_root(self) -> Node:
        """
        Get the root node.

        Returns:
            Root node

        Raises:
            NodeNotFoundError: If root is not set
        """
        root = await self._bg(_db.get_root_sync)
        if not root:
            raise NodeNotFoundError("root")
        return root

    @override
    async def set_root(self, node: Node) -> None:
        """
        Set the root node.

        Args:
            node: Root node to set
        """
        await self._bg(_db.set_root_sync, node)

    @override
    async def get_current_cursor(self) -> str:
        """
        Get the current cursor.

        Returns:
            Current cursor, or empty string if not set
        """
        cursor = await self._bg(_db.get_current_cursor_sync)
        return "" if not cursor else cursor

    @override
    async def get_node_by_id(self, node_id: str) -> Node:
        """
        Get a node by ID.

        Args:
            node_id: Node ID

        Returns:
            Node

        Raises:
            NodeNotFoundError: If node not found
        """
        node = await self._bg(_db.get_node_by_id_sync, node_id)
        if not node:
            raise NodeNotFoundError(node_id)
        return node

    @override
    async def get_node_by_path(self, path: PurePath) -> Node:
        """
        Get a node by path.

        Args:
            path: Absolute path

        Returns:
            Node

        Raises:
            NodeNotFoundError: If node not found
            ValueError: If path is not absolute
        """
        if not path.is_absolute():
            raise ValueError("Path must be absolute")

        node = await self._bg(_db.get_node_by_path_sync, path)
        if not node:
            raise NodeNotFoundError(str(path))
        return node

    @override
    async def resolve_path_by_id(self, node_id: str) -> PurePath:
        """
        Resolve the absolute path of a node by ID.

        Args:
            node_id: Node ID

        Returns:
            Absolute path

        Raises:
            NodeNotFoundError: If node not found
        """
        path = await self._bg(_db.resolve_path_by_id_sync, node_id)
        if not path:
            raise NodeNotFoundError(node_id)
        return path

    @override
    async def get_child_by_name(self, name: str, parent_id: str) -> Node:
        """
        Get a child node by name.

        Args:
            name: Child name
            parent_id: Parent node ID

        Returns:
            Child node

        Raises:
            NodeNotFoundError: If child not found
        """
        node = await self._bg(_db.get_child_by_name_sync, name, parent_id)
        if not node:
            raise NodeNotFoundError(name)
        return node

    @override
    async def get_children_by_id(self, parent_id: str) -> list[Node]:
        """
        Get all children of a node.

        Args:
            parent_id: Parent node ID

        Returns:
            List of child nodes
        """
        return await self._bg(_db.get_children_by_id_sync, parent_id)

    @override
    async def get_trashed_nodes(self) -> list[Node]:
        """
        Get all trashed nodes.

        Returns:
            List of trashed nodes
        """
        return await self._bg(_db.get_trashed_nodes_sync)

    @override
    async def apply_changes(
        self,
        changes: list[ChangeAction],
        cursor: str,
    ) -> None:
        """
        Apply a list of changes to the snapshot.

        Args:
            changes: List of changes (removes and updates)
            cursor: New cursor value
        """
        await self._bg(_db.apply_changes_sync, changes, cursor)

    @override
    async def find_nodes_by_regex(self, pattern: str) -> list[Node]:
        """
        Find nodes whose names match a regex pattern.

        Args:
            pattern: Regular expression pattern

        Returns:
            List of matching nodes
        """
        return await self._bg(_db.find_nodes_by_regex_sync, pattern)
