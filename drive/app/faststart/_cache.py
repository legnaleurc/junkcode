import sqlite3
from contextlib import closing, contextmanager
from logging import getLogger
from pathlib import Path

from wcpan.drive.core.types import Node


_L = getLogger(__name__)


def initialize_cache(dsn: Path):
    with migration_cache(dsn) as query:
        try:
            query.execute(
                """
                CREATE TABLE IF NOT EXISTS migrated (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE,
                    is_muxed INTEGER NOT NULL,
                    is_coded INTEGER NOT NULL
                );
                """
            )
            query.execute(
                """
                CREATE INDEX IF NOT EXISTS ix_migrated_node_id ON migrated(node_id);
                """
            )
        except sqlite3.OperationalError:
            _L.exception("cache initialize failed")


def is_migrated(dsn: Path, node: Node):
    with migration_cache(dsn) as query:
        query.execute(
            """
            SELECT id FROM migrated WHERE node_id = ? AND is_muxed = 1 AND is_coded = 1;
            """,
            (node.id,),
        )
        row = query.fetchone()
        if not row:
            return False
        return True


def has_cache(dsn: Path, node: Node):
    with migration_cache(dsn) as query:
        query.execute(
            """
            SELECT id FROM migrated WHERE node_id = ?;
            """,
            (node.id,),
        )
        row = query.fetchone()
        if not row:
            return False
        return True


def need_transcode(dsn: Path, node: Node):
    with migration_cache(dsn) as query:
        query.execute(
            """
            SELECT id FROM migrated WHERE node_id = ? AND is_coded = 0;
            """,
            (node.id,),
        )
        row = query.fetchone()
        if not row:
            return False
        return True


def set_cache(dsn: Path, node: Node, is_muxed: bool, is_coded: bool):
    with migration_cache(dsn) as query:
        query.execute(
            """
            SELECT is_muxed, is_coded FROM migrated WHERE node_id = ?;
            """,
            (node.id,),
        )
        row = query.fetchone()
        if not row:
            query.execute(
                """
                INSERT INTO migrated (node_id, is_muxed, is_coded) VALUES (?, ?, ?);
                """,
                (node.id, is_muxed, is_coded),
            )
            return
        query.execute(
            """
            UPDATE migrated SET is_muxed = ?, is_coded = ? WHERE node_id = ?;
            """,
            (is_muxed, is_coded, node.id),
        )


def unset_cache(dsn: Path, node: Node):
    with migration_cache(dsn) as query:
        query.execute(
            """
            DELETE FROM migrated WHERE node_id = ?;
            """,
            (node.id,),
        )


@contextmanager
def migration_cache(dsn: Path):
    with sqlite3.connect(dsn) as db, closing(db.cursor()) as query:
        yield query
