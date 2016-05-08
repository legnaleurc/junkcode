import os.path as op

from PyQt5 import QtCore
from acdcli.cache.db import NodeCache


class ACDModel(QtCore.QAbstractItemModel):

    def __init__(self, parent=None):
        super(ACDModel, self).__init__(parent)

        auth_folder = op.expanduser('~/.cache/acd_cli')
        self._db = NodeCache(auth_folder)
        self._root = Node(self._db)

    def columnCount(self, parent=QtCore.QModelIndex()):
        return 1

    def rowCount(self, parent=QtCore.QModelIndex()):
        if not parent.isValid():
            return len(self._root.children)

        node = parent.internalPointer()
        if not node.children:
            return 0
        return len(node.children)

    def index(self, row, column, parent=QtCore.QModelIndex()):
        if not parent.isValid():
            node = self._root
        else:
            node = parent.internalPointer()
        if not node.children or row >= len(node.children):
            return QtCore.QModelIndex()

        child = node.children[row]
        index = self.createIndex(row, column, child)
        return index

    def parent(self, index):
        if not index.isValid():
            return QtCore.QModelIndex()

        node = index.internalPointer()
        parent_node = node.parent

        if parent_node is self._root:
            return QtCore.QModelIndex()

        row = parent_node.index
        return self.createIndex(row, index.column(), parent_node)

    def data(self, index, role=QtCore.Qt.DisplayRole):
        if role != QtCore.Qt.DisplayRole:
            return QtCore.QVariant()

        if not index.isValid():
            return QtCore.QVariant()

        node = index.internalPointer()
        return node.name

    def flags(self, index):
        default_flags = super(ACDModel, self).flags(index)
        if index.isValid():
            return QtCore.Qt.ItemIsDragEnabled | QtCore.Qt.ItemIsDropEnabled | default_flags
        else:
            return QtCore.Qt.ItemIsDropEnabled | default_flags


class Node(object):

    def __init__(self, db, acdnode=None, parent=None, index=None):
        self._db = db
        if acdnode is None:
            self._node = self._db.get_root_node()
        else:
            self._node = acdnode
        self._parent = parent
        self._index = index
        self._children = None
        self._folders = None
        self._files = None

    @property
    def children(self):
        if not self._node or not self._node.is_folder:
            return None
        if self._children is None:
            self._load_children()
        return self._children

    @property
    def parent(self):
        return self._parent

    @property
    def index(self):
        return self._index

    @property
    def name(self):
        return self._node.name

    def _load_children(self):
        folders, files = self._db.list_children(self._node.id)
        self._folders = [Node(self._db, _, self, i) for i, _ in enumerate(folders)]
        offset = len(self._folders)
        self._files = [Node(self._db, _, self, offset + i) for i, _ in enumerate(files)]
        self._children = self._folders + self._files
