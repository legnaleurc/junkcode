"""
SQL schema definitions for SQLite snapshot storage.
"""

from typing import Literal, TypedDict


class NodeRow(TypedDict):
    """TypedDict for node rows from database."""

    id: str
    parent_id: str | None
    name: str
    is_directory: Literal[0, 1]
    is_trashed: Literal[0, 1]
    ctime: str
    mtime: str
    mime_type: str
    hash: str
    size: int
    is_image: Literal[0, 1]
    is_video: Literal[0, 1]
    width: int
    height: int
    ms_duration: int
    private: str | None


CURRENT_SCHEMA_VERSION = 1

CREATE_TABLES = """
CREATE TABLE IF NOT EXISTS metadata (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS nodes (
    id TEXT PRIMARY KEY,
    parent_id TEXT,
    name TEXT NOT NULL,
    is_directory INTEGER NOT NULL,
    is_trashed INTEGER NOT NULL,
    ctime TEXT NOT NULL,
    mtime TEXT NOT NULL,
    mime_type TEXT NOT NULL,
    hash TEXT NOT NULL,
    size INTEGER NOT NULL,
    is_image INTEGER NOT NULL,
    is_video INTEGER NOT NULL,
    width INTEGER NOT NULL,
    height INTEGER NOT NULL,
    ms_duration INTEGER NOT NULL,
    private TEXT,
    FOREIGN KEY (parent_id) REFERENCES nodes(id)
);

CREATE INDEX IF NOT EXISTS idx_nodes_parent_id ON nodes(parent_id);
CREATE INDEX IF NOT EXISTS idx_nodes_name ON nodes(name);
CREATE INDEX IF NOT EXISTS idx_nodes_trashed ON nodes(is_trashed);
CREATE INDEX IF NOT EXISTS idx_nodes_is_directory ON nodes(is_directory);
"""

# SQL queries
SQL_SELECT_NODE_BY_ID = """
SELECT * FROM nodes WHERE id = ?;
"""

SQL_SELECT_CHILDREN_BY_PARENT_ID = """
SELECT * FROM nodes WHERE parent_id = ?;
"""

SQL_SELECT_TRASHED_NODES = """
SELECT * FROM nodes WHERE is_trashed = 1;
"""

SQL_INSERT_OR_REPLACE_NODE = """
INSERT OR REPLACE INTO nodes (
    id, parent_id, name, is_directory, is_trashed,
    ctime, mtime, mime_type, hash, size,
    is_image, is_video, width, height, ms_duration, private
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
"""

SQL_DELETE_NODE = """
DELETE FROM nodes WHERE id = ?;
"""

SQL_GET_METADATA = """
SELECT value FROM metadata WHERE key = ?;
"""

SQL_SET_METADATA = """
INSERT OR REPLACE INTO metadata (key, value) VALUES (?, ?);
"""

SQL_DELETE_METADATA = """
DELETE FROM metadata WHERE key = ?;
"""
