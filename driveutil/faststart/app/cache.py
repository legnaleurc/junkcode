import contextlib
import sqlite3

from wcpan.drive.core.drive import Node


def initialize_cache():
    with migration_cache() as query:
        try:
            query.execute('''
                CREATE TABLE migrated (
                    id INTEGER PRIMARY KEY,
                    node_id TEXT NOT NULL UNIQUE,
                    size INTEGER NOT NULL,
                    created_at INTEGER NOT NULL
                );
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_node_id ON migrated(node_id);
            ''')
            query.execute('''
                CREATE INDEX ix_migrated_created_at ON migrated(created_at);
            ''')
        except sqlite3.OperationalError as e:
            print(e)
            pass


def is_migrated(node: Node):
    with migration_cache() as query:
        query.execute('''
            SELECT id FROM migrated WHERE node_id = ?;
        ''', (node.id_,))
        row = query.fetchone()
        if not row:
            return False
        return True


def set_migrated(node: Node):
    with migration_cache() as query:
        query.execute('''
            INSERT INTO migrated (node_id, size, created_at) VALUES (?, ?, ?);
        ''', (node.id_, node.size, node.created.timestamp))


@contextlib.contextmanager
def migration_cache():
    with sqlite3.connect('./data/_migrated.sqlite') as db, \
         contextlib.closing(db.cursor()) as query:
        yield query
