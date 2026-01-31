"""
Synchronous database operations for SQLite snapshot storage.
These functions run in a ThreadPoolExecutor to avoid blocking.
"""

import json
import sqlite3
from contextlib import contextmanager
from datetime import UTC, datetime
from pathlib import Path, PurePath
from typing import Generator

from wcpan.drive.core.lib import dispatch_change
from wcpan.drive.core.types import ChangeAction, Node

from ._sql import (
    CREATE_TABLES,
    CURRENT_SCHEMA_VERSION,
    SQL_DELETE_NODE,
    SQL_GET_METADATA,
    SQL_INSERT_OR_REPLACE_NODE,
    SQL_SELECT_CHILDREN_BY_PARENT_ID,
    SQL_SELECT_NODE_BY_ID,
    SQL_SELECT_TRASHED_NODES,
    SQL_SET_METADATA,
    NodeRow,
)


KEY_ROOT_ID = "root_id"
KEY_CURSOR = "cursor"


@contextmanager
def _connect(db_path: str) -> Generator[sqlite3.Connection, None, None]:
    """
    Connect to SQLite database.

    Args:
        db_path: str to database file (string)

    Yields:
        Database connection
    """
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    try:
        yield conn
    finally:
        conn.close()


def init_db(db_path: str) -> None:
    """
    Initialize the database schema.

    Args:
        db_path: str to database file (string)
    """
    with _connect(db_path) as conn:
        # Check schema version
        cursor = conn.execute("PRAGMA user_version;")
        row = cursor.fetchone()
        version = row[0] if row else 0

        if version != 0 and version != CURRENT_SCHEMA_VERSION:
            raise Exception(
                f"Schema version mismatch: expected {CURRENT_SCHEMA_VERSION}, got {version}"
            )

        # Create tables
        conn.executescript(CREATE_TABLES)

        # Set schema version
        conn.execute(f"PRAGMA user_version = {CURRENT_SCHEMA_VERSION};")
        conn.commit()


def get_root_sync(db_path: str) -> Node | None:
    """
    Get the root node.

    Args:
        db_path: str to database file

    Returns:
        Root node or None if not set
    """
    with _connect(db_path) as conn:
        # Get root ID from metadata
        cursor = conn.execute(SQL_GET_METADATA, (KEY_ROOT_ID,))
        row = cursor.fetchone()

        if not row or not row[0]:
            return None

        root_id = row[0]

        # Get root node
        cursor = conn.execute(SQL_SELECT_NODE_BY_ID, (root_id,))
        row = cursor.fetchone()

        if not row:
            return None

        return _node_from_row(row)


def set_root_sync(db_path: str, node: Node) -> None:
    """
    Set the root node.

    Args:
        db_path: str to database file
        node: Root node to set
    """
    with _connect(db_path) as conn:
        # Insert/update root node
        _insert_node(conn, node)

        # Update root ID in metadata
        conn.execute(SQL_SET_METADATA, (KEY_ROOT_ID, node.id))

        conn.commit()


def get_current_cursor_sync(db_path: str) -> str:
    """
    Get the current cursor.

    Args:
        db_path: str to database file

    Returns:
        Current cursor, or empty string if not set
    """
    with _connect(db_path) as conn:
        cursor = conn.execute(SQL_GET_METADATA, (KEY_CURSOR,))
        row = cursor.fetchone()

        if not row or not row[0]:
            return ""

        return row[0]


def get_node_by_id_sync(db_path: str, node_id: str) -> Node | None:
    """
    Get a node by ID.

    Args:
        db_path: str to database file
        node_id: Node ID

    Returns:
        Node or None if not found
    """
    with _connect(db_path) as conn:
        cursor = conn.execute(SQL_SELECT_NODE_BY_ID, (node_id,))
        row = cursor.fetchone()

        if not row:
            return None

        return _node_from_row(row)


def get_children_by_id_sync(db_path: str, parent_id: str) -> list[Node]:
    """
    Get all children of a node.

    Args:
        db_path: str to database file
        parent_id: Parent node ID

    Returns:
        List of child nodes
    """
    with _connect(db_path) as conn:
        cursor = conn.execute(SQL_SELECT_CHILDREN_BY_PARENT_ID, (parent_id,))
        return [_node_from_row(row) for row in cursor]


def get_node_by_path_sync(db_path: str, path: PurePath) -> Node | None:
    """
    Get a node by path.

    Args:
        db_path: str to database file
        path: Absolute path

    Returns:
        Node or None if not found
    """
    if not path.is_absolute():
        raise ValueError("Path must be absolute")

    with _connect(db_path) as conn:
        # Get root node
        cursor = conn.execute(SQL_GET_METADATA, (KEY_ROOT_ID,))
        row = cursor.fetchone()

        if not row or not row[0]:
            return None

        current_id = row[0]

        # Traverse path
        parts = path.parts[1:]  # Skip leading "/"
        for part in parts:
            cursor = conn.execute(
                "SELECT id FROM nodes WHERE parent_id = ? AND name = ?;",
                (current_id, part),
            )
            row = cursor.fetchone()

            if not row:
                return None

            current_id = row[0]

        # Get final node
        return get_node_by_id_sync(db_path, current_id)


def resolve_path_by_id_sync(db_path: str, node_id: str) -> PurePath | None:
    """
    Resolve the path of a node by ID.

    Args:
        db_path: str to database file
        node_id: Node ID

    Returns:
        Absolute path or None if not found
    """
    with _connect(db_path) as conn:
        parts: list[str] = []
        current_id = node_id

        while True:
            cursor = conn.execute(SQL_SELECT_NODE_BY_ID, (current_id,))
            row = cursor.fetchone()

            if not row:
                return None

            name = row["name"]
            parent_id = row["parent_id"]

            # If no parent, we're at root
            if not parent_id:
                parts.insert(0, "/")
                break

            parts.insert(0, name)
            current_id = parent_id

        return PurePath(*parts)


def get_child_by_name_sync(db_path: str, name: str, parent_id: str) -> Node | None:
    """
    Get a child node by name.

    Args:
        db_path: str to database file
        name: Child name
        parent_id: Parent node ID

    Returns:
        Child node or None if not found
    """
    with _connect(db_path) as conn:
        cursor = conn.execute(
            "SELECT * FROM nodes WHERE parent_id = ? AND name = ?;",
            (parent_id, name),
        )
        row = cursor.fetchone()

        if not row:
            return None

        return _node_from_row(row)


def get_trashed_nodes_sync(db_path: str) -> list[Node]:
    """
    Get all trashed nodes.

    Args:
        db_path: str to database file

    Returns:
        List of trashed nodes
    """
    with _connect(db_path) as conn:
        cursor = conn.execute(SQL_SELECT_TRASHED_NODES)
        return [_node_from_row(row) for row in cursor]


def apply_changes_sync(db_path: str, changes: list[ChangeAction], cursor: str) -> None:
    """
    Apply a list of changes to the database.

    Args:
        db_path: str to database file
        changes: List of changes
        cursor: New cursor value
    """
    with _connect(db_path) as conn:
        for change in changes:
            dispatch_change(
                change,
                on_remove=lambda node_id: _delete_node(conn, node_id),
                on_update=lambda node: _insert_node(conn, node),
            )

        # Update cursor
        conn.execute(SQL_SET_METADATA, (KEY_CURSOR, cursor))
        conn.commit()


def find_nodes_by_regex_sync(db_path: str, pattern: str) -> list[Node]:
    """
    Find nodes matching a regex pattern.

    Args:
        db_path: str to database file
        pattern: Regex pattern

    Returns:
        List of matching nodes
    """
    import re

    regex = re.compile(pattern, re.IGNORECASE)

    with _connect(db_path) as conn:
        cursor = conn.execute("SELECT * FROM nodes;")
        nodes = []

        for row in cursor:
            node = _node_from_row(row)
            if regex.search(node.name):
                nodes.append(node)

        return nodes


def _node_from_row(row: sqlite3.Row) -> Node:
    """
    Convert a database row to a Node.

    Args:
        row: Database row

    Returns:
        Node object
    """
    # Parse private data (JSON)
    private = None
    if row["private"]:
        private = json.loads(row["private"])

    # Parse timestamps
    ctime = datetime.fromisoformat(row["ctime"])
    mtime = datetime.fromisoformat(row["mtime"])

    # Ensure UTC timezone
    if ctime.tzinfo is None:
        ctime = ctime.replace(tzinfo=UTC)
    if mtime.tzinfo is None:
        mtime = mtime.replace(tzinfo=UTC)

    return Node(
        id=row["id"],
        parent_id=row["parent_id"],
        name=row["name"],
        is_directory=bool(row["is_directory"]),
        is_trashed=bool(row["is_trashed"]),
        ctime=ctime,
        mtime=mtime,
        mime_type=row["mime_type"],
        hash=row["hash"],
        size=row["size"],
        is_image=bool(row["is_image"]),
        is_video=bool(row["is_video"]),
        width=row["width"],
        height=row["height"],
        ms_duration=row["ms_duration"],
        private=private,
    )


def _insert_node(conn: sqlite3.Connection, node: Node) -> None:
    """
    Insert or update a node in the database.

    Args:
        conn: Database connection
        node: Node to insert
    """
    # Serialize private data
    private_json = json.dumps(node.private) if node.private else None

    # Convert timestamps to ISO format
    ctime_str = node.ctime.isoformat()
    mtime_str = node.mtime.isoformat()

    conn.execute(
        SQL_INSERT_OR_REPLACE_NODE,
        (
            node.id,
            node.parent_id,
            node.name,
            int(node.is_directory),
            int(node.is_trashed),
            ctime_str,
            mtime_str,
            node.mime_type,
            node.hash,
            node.size,
            int(node.is_image),
            int(node.is_video),
            node.width,
            node.height,
            node.ms_duration,
            private_json,
        ),
    )


def _delete_node(conn: sqlite3.Connection, node_id: str) -> None:
    """
    Delete a node from the database.

    Args:
        conn: Database connection
        node_id: Node ID to delete
    """
    conn.execute(SQL_DELETE_NODE, (node_id,))
