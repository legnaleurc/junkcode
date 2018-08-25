import pathlib as pl

from PySide2 import QtCore, QtNetwork


class DriveModel(QtCore.QAbstractItemModel):

    def __init__(self, parent=None):
        super().__init__(parent)

        self._root = Node('/')
        self._setRootPath = False
        self._fileInfoGatherer = FileInfoGatherer(self)

        self._fileInfoGatherer.updates.connect(self._onFileSystemChanged)

    def setRootPath(self):
        self._setRootPath = True
        newRootIndex = QtCore.QModelIndex()
        self.fetchMore(newRootIndex)

    def filePath(self, index):
        if not index.isValid():
            return '/'

        path = []
        while index.isValid():
            dirNode = self._node(index)
            if dirNode:
                path.insert(0, dirNode.fileName)
            index = index.parent()
        path.insert(0, '/')
        fullPath = pl.Path(path)
        return str(fullPath)

    def index_by_path(self, path, column=0):
        node = self._node_by_path(path, False)
        return self._index_by_node(node, column)

    def _index_by_node(self, node, column):
        parentNode = node.parent if node else None
        if node is self._root or not parentNode:
            return QModelIndex()

        if not node.isVisible:
            return QModelIndex()

        visualRow = self._translateVisibleLocation(parentNode, parentNode.visibleLocation(node.fileName))
        return self.createIndex(visualRow, column, node)

    # override
    def fetchMore(self, parent):
        if not self._setRootPath:
            return

        indexNode = self._node(parent)
        if indexNode.populatedChildren:
            return
        indexNode.populatedChildren = True

        path = self.filePath(parent)
        self._fileInfoGatherer.list(path)

    # override
    def columnCount(self, parent):
        if parent.column() > 0:
            return 0
        return 1

    # override
    def rowCount(self, parent):
        if parent.column() > 0:
            return 0

        if not parent.isValid():
            return len(self._root.visibleChildren)

        parentNode = self._node(parent)
        return len(parentNode.visibleChildren)

    def _node(self, index):
        if not index.isValid():
            return self._root

        indexNode = index.internalPointer()
        assert indexNode
        return indexNode

    def _node_by_path(self, path, fetch):
        if not path:
            return self._root

        parent = self._root
        path = pl.Path(path)
        for part in path.parts[1:]:
            alreadyExisted = part in parent.children

            if alreadyExisted:
                node = parent.children[part]
            else:
                # TODO check if the path exists
                node = self._addNode(parent, part)

            if not node.isVisible:
                self._addVisibleFiles(parent, [part])

            parent = node

        return parent

    def _addNode(self, parentNode, fileName):
        node = Node(fileName, parentNode)
        parentNode.children[fileName] = node
        return node

    def _addVisibleFiles(self, parentNode, newFiles):
        parent = self.index(parentNode)
        self.beginInsertRows(parent, len(parentNode.visibleChildren),
                             len(parentNode.visibleChildren) + len(newFiles) - 1)

        if parentNode.dirtyChildrenIndex == -1:
            parentNode.dirtyChildrenIndex = len(parentNode.visibleChildren)

        for newFile in newFiles:
            parentNode.visibleChildren.append(newFile)
            parentNode.children[newFile].isVisible = True

        self.endInsertRows()

    @QtCore.Slot(str, QtCore.QJsonDocument)
    def _onFileSystemChanged(self, path, files):
        parentNode = self._node_by_path(path, False)
        parentIndex = self.index(parentNode)


class Node:

    def __init__(self, fileName, parentNode=None):
        self.fileName = fileName
        self.parent = parentNode
        self.visibleChildren = []
        self.children = {}
        self.populatedChildren = False
        self.dirtyChildrenIndex = -1
        self.isVisible = False


class FileInfoGatherer(QtCore.QObject):

    Category = QtNetwork.QNetworkRequest.User
    Path = QtNetwork.QNetworkRequest.Attribute(QtNetwork.QNetworkRequest.User + 1)

    updates = QtCore.Signal(str, QtCore.QJsonDocument)

    def __init__(self, parent=None):
        super().__init__(parent)

        self._curl = QtNetwork.QNetworkAccessManager(self)

    def list(self, directoryPath):
        query = QtCore.QUrlQuery()
        query.addQueryItem('path', directoryPath)
        url = QtCore.QUrl('http://localhost:8000/api/v1/list')
        url.setQuery(query)
        request = QtNetwork.QNetworkRequest(url)
        request.setAttribute(self.Category, 'list')
        request.setAttribute(self.Path, directoryPath)
        reply = self._curl.get(request)
        reply.finished.connect(self._onFinished)

    @QtCore.Slot()
    def _onFinished(self):
        reply = self.sender()

        action = reply.request().attribute(self.Category)
        if action == 'list':
            path = reply.request().attribute(self.Path)
            json = reply.readAll()
            json = QtCore.QJsonDocument.fromJson(json)
            self.updates.emit(path, json)

        reply.deleteLater()
