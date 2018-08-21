import pathlib as pl

from PySide2 import QtCore, QtNetwork


class DriveModel(QtCore.QAbstractItemModel):

    def __init__(self, parent=None):
        super().__init__(parent)

        self._root = Node('/')
        self._setRootPath = False
        self._fileInfoGatherer = FileInfoGatherer(self)

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


class Node:

    def __init__(self, fileName):
        self.fileName = fileName
        self.visibleChildren = []
        self.populatedChildren = False


class FileInfoGatherer(QtCore.QObject):

    Category = QtNetwork.QNetworkRequest.User
    Path = QtNetwork.QNetworkRequest.User + 1

    updates = QtCore.Signal()

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

        action = reply.request().getAttribute(self.Category)
        if action == 'list':
            path = reply.request().getAttribute(self.Path)
            json = reply.readAll()
            json = QtCore.QJsonDocument.fromJson(json)
            self.updates.emit(path, json)

        reply.deleteLater()
