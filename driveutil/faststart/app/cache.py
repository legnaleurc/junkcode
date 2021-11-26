import contextlib
import sqlite3

from wcpan.drive.core.drive import Node
from wcpan.logger import DEBUG


def initialize_cache():
    with migration_cache() as query:
        try:
            query.execute('''
                CREATE TABLE migrated (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE,
                    is_muxed INTEGER NOT NULL,
                    is_coded INTEGER NOT NULL
                );
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_node_id ON migrated(node_id);
            ''')
        except sqlite3.OperationalError as e:
            DEBUG('faststart') << e


def is_migrated(node: Node):
    with migration_cache() as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ? AND is_muxed = 1 AND is_coded = 1;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def has_cache(node: Node):
    with migration_cache() as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def need_transcode(node: Node):
    with migration_cache() as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ? AND is_coded = 0;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def set_migrated(node: Node, is_muxed: bool, is_coded: bool):
    with migration_cache() as query:
        query.execute('''
            SELECT is_muxed, is_coded FROM migrated WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            query.execute('''
                INSERT INTO migrated (node_id, is_muxed, is_coded) VALUES (?, ?, ?);
            ''', (node.id_, is_muxed, is_coded))
            return
        query.execute('''
            UPDATE migrated SET is_muxed = ?, is_coded = ? WHERE node_id = ?;
        ''', (is_muxed, is_coded, node.id_))


@contextlib.contextmanager
def migration_cache():
    with sqlite3.connect('./data/_migrated.sqlite') as db, \
         contextlib.closing(db.cursor()) as query:
        yield query
