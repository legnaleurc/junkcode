import contextlib as cl
import datetime as dt
import pathlib as pl
import re
import sqlite3

from . import util as u


SQL_CREATE = '''
    CREATE TABLE metadata (
        key VARCHAR(64) NOT NULL,
        value VARCHAR(4096),
        PRIMARY KEY (key)
    );

    CREATE TABLE nodes (
        id VARCHAR(32) NOT NULL,
        name VARCHAR(4096),
        status VARCHAR(16),
        created DATETIME,
        modified DATETIME,
        PRIMARY KEY (id),
        UNIQUE (id),
        CHECK (status IN ('AVAILABLE', 'TRASH'))
    );

    CREATE TABLE files (
        id VARCHAR(32) NOT NULL,
        md5 VARCHAR(32),
        size BIGINT,
        PRIMARY KEY (id),
        UNIQUE (id),
        FOREIGN KEY (id) REFERENCES nodes (id)
    );

    CREATE TABLE parentage (
        parent VARCHAR(32) NOT NULL,
        child VARCHAR(32) NOT NULL,
        PRIMARY KEY (parent, child),
        FOREIGN KEY (child) REFERENCES nodes (id)
    );

    CREATE INDEX ix_parentage_child ON parentage(child);
    CREATE INDEX ix_nodes_names ON nodes(name);
    PRAGMA user_version = 1;
'''


CURRENT_SCHEMA_VERSION = 1


class Database(object):

    def __init__(self, settings):
        self._settings = settings

        path = settings['nodes_database_file']
        self._db = sqlite3.connect(path, detect_types=sqlite3.PARSE_DECLTYPES)
        self._db.row_factory = sqlite3.Row
        self._db.create_function('REGEXP', 2, sqlite3_regexp)

        self._metadata = Metadata(self._db)

    def close(self):
        self._db.close()

    def initialize(self):
        try:
            self._try_create()
        except sqlite3.OperationalError as e:
            pass
        with ReadOnly(self._db) as query:
            query.execute('PRAGMA user_version;')
            rv = query.fetchone()
        version = rv[0]

        if CURRENT_SCHEMA_VERSION > version:
            self._migrate(version)

    @property
    def metadata(self):
        return self._metadata

    @property
    def root_id(self):
        return self.metadata['root_id']

    @property
    def root_node(self):
        return self.get_node_by_id(self.root_id)

    def get_node_by_id(self, node_id):
        with ReadOnly(self._db) as query:
            query.execute('''
                SELECT id, name, status, created, modified
                FROM nodes
                WHERE id=?
            ;''', (node_id,))
            rv = query.fetchone()
            if not rv:
                return None
            node = dict(rv)

            query.execute('''
                SELECT id, md5, size
                FROM files
                WHERE id=?
            ;''', (node_id,))
            rv = query.fetchone()
            is_folder = rv is None
            node['is_folder'] = is_folder
            node['md5'] = None if is_folder else rv['md5']
            node['size'] = None if is_folder else rv['size']

            query.execute('''
                SELECT parent, child
                FROM parentage
                WHERE child=?
            ;''', (node_id,))
            rv = query.fetchall()
            node['parents'] = [_['parent'] for _ in rv]

            node = Node.from_database(node)
            return node

    def get_node_by_path(self, path):
        path = pl.Path(path)
        parts = list(path.parts)
        if parts[0] != '/':
            raise Exception('invalid path')

        node_id = self.root_id
        parts.pop(0)
        with ReadOnly(self._db) as query:
            for part in parts:
                query.execute('''
                    SELECT nodes.id AS id
                    FROM parentage
                        INNER JOIN nodes ON parentage.child=nodes.id
                    WHERE parentage.parent=? AND nodes.name=?
                ;''', (node_id, part))
                rv = query.fetchone()
                if not rv:
                    return None
                node_id = rv['id']

            node = self.get_node_by_id(node_id)
        return node

    def get_path_by_id(self, node_id):
        parts = []
        with ReadOnly(self._db) as query:
            while True:
                query.execute('''
                    SELECT name
                    FROM nodes
                    WHERE id=?
                ;''', (node_id,))
                rv = query.fetchone()
                if not rv:
                    return None

                name = rv['name']
                if not name:
                    parts.insert(0, '/')
                    break
                parts.insert(0, name)

                query.execute('''
                    SELECT parent
                    FROM parentage
                    WHERE child=?
                ;''', (node_id,))
                rv = query.fetchone()
                node_id = rv['parent']

        path = pl.Path(*parts)
        return str(path)

    def get_child_by_id(self, node_id, name):
        with ReadOnly(self._db) as query:
            query.execute('''
                SELECT nodes.id AS id
                FROM nodes
                    INNER JOIN parentage ON parentage.child=nodes.id
                WHERE parentage.parent=? AND nodes.name=?
            ;''', (node_id, name))
            rv = query.fetchone()

        if not rv:
            return None

        node = self.get_node_by_id(rv['id'])
        return node

    def get_children_by_id(self, node_id):
        with ReadOnly(self._db) as query:
            query.execute('''
                SELECT child
                FROM parentage
                WHERE parent=?
            ;''', (node_id,))
            rv = query.fetchall()

        children = [self.get_node_by_id(_['child']) for _ in rv]
        return children

    def apply_changes(self, changes, check_point):
        with ReadWrite(self._db) as query:
            for change in changes:
                is_removed = change['removed']
                if is_removed:
                    inner_delete_node_by_id(query, change['fileId'])
                else:
                    node = Node.from_api(change['file'])
                    inner_insert_node(query, node)

            self.metadata['check_point'] = check_point

    def insert_node(self, node):
        with ReadWrite(self._db) as query:
            inner_insert_node(query, node)

        if not node.name:
            self.metadata['root_id'] = node.id_

    def find_duplicate_nodes(self):
        with ReadOnly(self._db) as query:
            query.execute('''
                SELECT nodes.name AS name, parentage.parent as parent_id
                FROM nodes
                    INNER JOIN parentage ON parentage.child=nodes.id
                GROUP BY parentage.parent, nodes.name
                HAVING COUNT(*) > 1
            ;''')
            name_parent_pair = query.fetchall()

            node_id_list = []
            for pair in name_parent_pair:
                query.execute('''
                    SELECT nodes.id AS id
                    FROM nodes
                        INNER JOIN parentage ON parentage.child=nodes.id
                    WHERE parentage.parent=? AND nodes.name=?
                ;''', (pair['parent_id'], pair['name']))
                rv = query.fetchall()
                rv = (_['id'] for _ in rv)
                node_id_list.extend(rv)

            rv = [self.get_node_by_id(_) for _ in node_id_list]
        return rv

    def _try_create(self):
        with ReadWrite(self._db) as query:
            query.executescript(SQL_CREATE)

    def _migrate(self, version):
        raise NotImplementedError()


class Metadata(object):

    def __init__(self, db):
        self._db = db

    def __getitem__(self, key):
        with ReadOnly(self._db) as query:
            query.execute('SELECT value FROM metadata WHERE key = ?;', (key,))
            rv = query.fetchone()
        if not rv:
            raise KeyError
        return rv['value']

    def __setitem__(self, key, value):
        with ReadWrite(self._db) as query:
            query.execute('INSERT OR REPLACE INTO metadata VALUES (?, ?);', (key, value))


class Node(object):

    @staticmethod
    def from_api(data):
        node = Node(data)
        node._initialize_from_api()
        return node

    @staticmethod
    def from_database(data):
        node = Node(data)
        node._initialize_from_database()
        return node

    def __init__(self, data):
        self._data = data

    @property
    def is_root(self):
        return self._name is None

    @property
    def is_folder(self):
        return self._is_folder

    @property
    def id_(self):
        return self._id

    @property
    def name(self):
        return self._name

    @property
    def status(self):
        return self._status

    @property
    def available(self):
        return self._status == 'AVAILABLE'

    @property
    def trashed(self):
        return self._status == 'TRASH'

    @property
    def created(self):
        return self._created

    @property
    def modified(self):
        return self._modified

    @property
    def parents(self):
        return self._parents

    @property
    def parent_id(self):
        if not self._parents:
            return None
        return self._parents[0]

    @property
    def md5(self):
        return self._md5

    @property
    def size(self):
        return self._size

    def _initialize_from_api(self):
        data = self._data
        self._id = data['id']
        self._name = data['name']
        self._status = 'TRASH' if data['trashed'] else 'AVAILABLE'
        self._created = u.from_isoformat(data['createdTime'])
        self._modified = u.from_isoformat(data['modifiedTime'])
        self._parents = data['parents']

        self._is_folder = data['mimeType'] == u.FOLDER_MIME_TYPE
        self._md5 = data.get('md5Checksum', None)
        self._size = data.get('size', None)

    def _initialize_from_database(self):
        data = self._data
        self._id = data['id']
        self._name = data['name']
        self._status = data['status']
        self._created = data['created']
        self._modified = data['modified']
        self._parents = data['parents']

        self._is_folder = data['is_folder']
        self._md5 = data['md5']
        self._size = data['size']


class ReadOnly(object):

    def __init__(self, db):
        self._db = db

    def __enter__(self):
        self._cursor = self._db.cursor()
        return self._cursor

    def __exit__(self, exc_type, exc_value, exc_tb):
        self._cursor.close()


class ReadWrite(object):

    def __init__(self, db):
        self._db = db

    def __enter__(self):
        self._cursor = self._db.cursor()
        return self._cursor

    def __exit__(self, exc_type, exc_value, exc_tb):
        if exc_type is None:
            self._db.commit()
        else:
            self._db.rollback()
        self._cursor.close()


def inner_insert_node(query, node):
    # add this node
    query.execute('''
        INSERT OR REPLACE INTO nodes
        (id, name, status, created, modified)
        VALUES
        (?, ?, ?, ?, ?)
    ;''', (node.id_, node.name, node.status, node.created, node.modified))

    # add file information
    if not node.is_folder:
        query.execute('''
            INSERT OR REPLACE INTO files
            (id, md5, size)
            VALUES
            (?, ?, ?)
        ;''', (node.id_, node.md5, node.size))

    # add parentage
    for parent in node.parents:
        query.execute('''
            INSERT OR REPLACE INTO parentage
            (parent, child)
            VALUES
            (?, ?)
        ;''', (parent, node.id_))


def inner_delete_node_by_id(query, node_id):
    # disconnect parents
    query.execute('''
        DELETE FROM parentage
        WHERE child=?
    ;''', (node_id,))

    # remove from files
    query.execute('''
        DELETE FROM files
        WHERE id=?
    ;''', (node_id,))

    # remove from nodes
    query.execute('''
        DELETE FROM nodes
        WHERE id=?
    ;''', (node_id,))


def sqlite3_regexp(pattern, cell):
    if cell is None:
        # root node
        return False
    return re.search(pattern, cell) is not None


def initialize():
    sqlite3.register_adapter(dt.datetime, lambda _: _.isoformat())
    def to_dt(raw_datetime):
        s = raw_datetime.decode('utf-8')
        return u.from_isoformat(s)
    sqlite3.register_converter('DATETIME', to_dt)


initialize()
