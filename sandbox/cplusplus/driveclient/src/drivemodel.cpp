/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "drivemodel_p.h"
#include "drivemodel.h"
#include <QtCore/QLocale>
#include <QtCore/QCollator>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtWidgets/QApplication>
#include <QtCore/QtDebug>

#if QT_CONFIG(messagebox)
#include <QtWidgets/QMessageBox>
#endif

#include <algorithm>


const static int NodeRole = Qt::UserRole + 4;


QT_BEGIN_NAMESPACE

/*!
    \enum DriveModel::Roles
    \value FileIconRole
    \value FilePathRole
    \value FileNameRole
    \value FilePermissions
*/

/*!
    \class DriveModel
    \since 4.4

    \brief The DriveModel class provides a data model for the local filesystem.

    \ingroup model-view
    \inmodule QtWidgets

    This class provides access to the local filesystem, providing functions
    for renaming and removing files and directories, and for creating new
    directories. In the simplest case, it can be used with a suitable display
    widget as part of a browser or filter.

    DriveModel can be accessed using the standard interface provided by
    QAbstractItemModel, but it also provides some convenience functions that are
    specific to a directory model.
    The fileInfo(), isDir(), fileName() and filePath() functions provide information
    about the underlying files and directories related to items in the model.
    Directories can be created and removed using mkdir(), rmdir().

    \note DriveModel requires an instance of \l QApplication.

    \section1 Example Usage

    A directory model that displays the contents of a default directory
    is usually constructed with a parent object:

    \snippet shareddirmodel/main.cpp 2

    A tree view can be used to display the contents of the model

    \snippet shareddirmodel/main.cpp 4

    and the contents of a particular directory can be displayed by
    setting the tree view's root index:

    \snippet shareddirmodel/main.cpp 7

    The view's root index can be used to control how much of a
    hierarchical model is displayed. DriveModel provides a convenience
    function that returns a suitable model index for a path to a
    directory within the model.

    \section1 Caching and Performance

    DriveModel will not fetch any files or directories until setRootPath()
    is called.  This will prevent any unnecessary querying on the file system
    until that point such as listing the drives on Windows.

    Unlike QDirModel, DriveModel uses a separate thread to populate
    itself so it will not cause the main thread to hang as the file system
    is being queried.  Calls to rowCount() will return 0 until the model
    populates a directory.

    DriveModel keeps a cache with file information. The cache is
    automatically kept up to date using the QFileSystemWatcher.

    \sa {Model Classes}
*/

/*!
    \fn bool DriveModel::rmdir(const QModelIndex &index)

    Removes the directory corresponding to the model item \a index in the
    file system model and \b{deletes the corresponding directory from the
    file system}, returning true if successful. If the directory cannot be
    removed, false is returned.

    \warning This function deletes directories from the file system; it does
    \b{not} move them to a location where they can be recovered.

    \sa remove()
*/

/*!
    \fn QIcon DriveModel::fileName(const QModelIndex &index) const

    Returns the file name for the item stored in the model under the given
    \a index.
*/

/*!
    \fn QIcon DriveModel::fileIcon(const QModelIndex &index) const

    Returns the icon for the item stored in the model under the given
    \a index.
*/

/*!
    \fn DriveFileInfo DriveModel::fileInfo(const QModelIndex &index) const

    Returns the DriveFileInfo for the item stored in the model under the given
    \a index.
*/
DriveFileInfo DriveModel::fileInfo(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    return d->node(index)->info;
}

/*!
    \fn void DriveModel::rootPathChanged(const QString &newPath);

    This signal is emitted whenever the root path has been changed to a \a newPath.
*/

/*!
    \fn void DriveModel::fileRenamed(const QString &path, const QString &oldName, const QString &newName)

    This signal is emitted whenever a file with the \a oldName is successfully
    renamed to \a newName.  The file is located in in the directory \a path.
*/

/*!
    \since 4.7
    \fn void DriveModel::directoryLoaded(const QString &path)

    This signal is emitted when the gatherer thread has finished to load the \a path.

*/

/*!
    \fn bool DriveModel::remove(const QModelIndex &index)

    Removes the model item \a index from the file system model and \b{deletes the
    corresponding file from the file system}, returning true if successful. If the
    item cannot be removed, false is returned.

    \warning This function deletes files from the file system; it does \b{not}
    move them to a location where they can be recovered.

    \sa rmdir()
*/

bool DriveModel::remove(const QModelIndex &aindex)
{
    Q_D(DriveModel);

    // TODO
    return false;
    // const QString path = d->filePath(aindex);
    // const DriveFileInfo fileInfo(&d->driveSystem, path);
    // const bool success = (fileInfo.isFile() || fileInfo.isSymLink())
    //         ? QFile::remove(path) : QDir(path).removeRecursively();
    // return success;
}

/*!
  Constructs a file system model with the given \a parent.
*/
DriveModel::DriveModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new DriveModelPrivate(this))
{
    Q_D(DriveModel);
    d->init();
}

/*!
    \internal
*/
DriveModel::DriveModel(DriveModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(&dd)
{
    Q_D(DriveModel);
    d->init();
}

/*!
  Destroys this file system model.
*/
DriveModel::~DriveModel()
{
}

/*!
    \reimp
*/
QModelIndex DriveModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const DriveModel);

    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    // get the parent node
    auto parentNode = (d->indexValid(parent) ? d->node(parent) : d->driveSystem->root());
    Q_ASSERT(parentNode);

    // now get the internal pointer for the index
    const int i = d->translateVisibleLocation(parentNode, row);
    if (i >= parentNode->visibleChildren.size())
        return QModelIndex();
    const QString & childId = parentNode->visibleChildren.at(i);
    DriveNodeCSP indexNode = parentNode->child(childId);
    Q_ASSERT(indexNode);

    return this->createIndex(row, column, const_cast<DriveNode*>(indexNode.get()));
}

/*!
    \reimp
*/
QModelIndex DriveModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (row == idx.row() && column < DriveModelPrivate::NumColumns) {
        // cheap sibling operation: just adjust the column:
        return createIndex(row, column, idx.internalPointer());
    } else {
        // for anything else: call the default implementation
        // (this could probably be optimized, too):
        return QAbstractItemModel::sibling(row, column, idx);
    }
}

/*!
    \overload

    Returns the model item index for the given \a path and \a column.
*/
QModelIndex DriveModel::index(const QString & id, int column) const
{
    Q_D(const DriveModel);
    auto node = d->node(id, false);
    return d->index(node, column);
}

/*!
    \internal

    Return the FileNode that goes to index.
  */
DriveNodeSP DriveModelPrivate::node(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return this->driveSystem->root();
    }
    return index.data(NodeRole).value<DriveNodeSP>();
}

/*!
    \internal

    Given a path return the matching FileNode or &root if invalid
*/
DriveNodeSP DriveModelPrivate::node(const QString & id, bool fetch) const
{
    if (id.isEmpty()) {
        return this->driveSystem->root();
    }

    if (!fetch) {
        return this->driveSystem->node(id);
    }

    auto info = this->driveSystem->info(id);
    return this->driveSystem->upsertNode(info);
}

/*!
    \reimp
*/
void DriveModel::timerEvent(QTimerEvent *event)
{
    Q_D(DriveModel);
    if (event->timerId() == d->fetchingTimer.timerId()) {
        d->fetchingTimer.stop();
#ifndef QT_NO_FILESYSTEMWATCHER
        for (int i = 0; i < d->toFetch.count(); ++i) {
            DriveNodeCSP node = d->toFetch.at(i).node;
            if (!node->hasInformation()) {
                d->fileInfoGatherer.fetchExtendedInformation(d->toFetch.at(i).dir,
                                                 QStringList(d->toFetch.at(i).file));
            } else {
                // qDebug("yah!, you saved a little gerbil soul");
            }
        }
#endif
        d->toFetch.clear();
    }
}

/*!
    Returns \c true if the model item \a index represents a directory;
    otherwise returns \c false.
*/
bool DriveModel::isDir(const QModelIndex &index) const
{
    // This function is for public usage only because it could create a file info
    Q_D(const DriveModel);
    if (!index.isValid())
        return true;
    DriveNodeSP n = d->node(index);
    if (n->hasInformation())
        return n->info.isDir();
    return fileInfo(index).isDir();
}

/*!
    Returns the size in bytes of \a index. If the file does not exist, 0 is returned.
  */
qint64 DriveModel::size(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!index.isValid())
        return 0;
    return d->node(index)->info.size();
}

/*!
    Returns the type of file \a index such as "Directory" or "JPEG file".
  */
QString DriveModel::type(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!index.isValid())
        return QString();
    return d->node(index)->info.mimeType();
}

/*!
    Returns the date and time when \a index was last modified.
 */
QDateTime DriveModel::lastModified(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!index.isValid())
        return QDateTime();
    return d->node(index)->info.lastModified();
}

/*!
    \reimp
*/
QModelIndex DriveModel::parent(const QModelIndex &index) const
{
    Q_D(const DriveModel);

    if (!d->indexValid(index)) {
        return QModelIndex();
    }

    DriveNodeSP indexNode = d->node(index);
    Q_ASSERT(indexNode);
    DriveNodeSP parentNode = indexNode->parent.lock();
    if (!parentNode || parentNode == d->driveSystem->root()) {
        return QModelIndex();
    }

    // get the parent's row
    DriveNodeSP grandParentNode = parentNode->parent.lock();
    Q_ASSERT(grandParentNode->children.count(parentNode->info.id()) == 1);
    auto location = grandParentNode->visibleLocation(parentNode->info.id());
    int visualRow = d->translateVisibleLocation(grandParentNode, location);
    if (visualRow == -1) {
        return QModelIndex();
    }
    return this->createIndex(visualRow, 0, parentNode.get());
}

/*
    \internal

    return the index for node
*/
QModelIndex DriveModelPrivate::index(DriveNodeCSP node, int column) const
{
    Q_Q(const DriveModel);

    if (!node || node == this->driveSystem->root()) {
        return QModelIndex();
    }
    DriveNodeSP parentNode = node->parent.lock();
    if (!parentNode) {
        return QModelIndex();
    }

    // get the parent's row
    if (!node->isVisible) {
        return QModelIndex();
    }

    auto location = parentNode->visibleLocation(node->info.id());
    int visualRow = translateVisibleLocation(parentNode, location);
    return q->createIndex(visualRow, column, const_cast<DriveNode *>(node.get()));
}

/*!
    \reimp
*/
bool DriveModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const DriveModel);

    if (parent.column() > 0) {
        return false;
    }

    // drives
    if (!parent.isValid()) {
        return true;
    }

    DriveNodeCSP indexNode = d->node(parent);
    Q_ASSERT(indexNode);
    return (indexNode->info.isDir());
}

/*!
    \reimp
 */
bool DriveModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const DriveModel);
    DriveNodeCSP indexNode = d->node(parent);
    return (!indexNode->populatedChildren);
}

/*!
    \reimp
 */
void DriveModel::fetchMore(const QModelIndex &parent)
{
    Q_D(DriveModel);

    if (!d->setRootPath) {
        return;
    }

    DriveNodeSP indexNode = d->node(parent);
    if (indexNode->populatedChildren) {
        return;
    }

    indexNode->populatedChildren = true;
#ifndef QT_NO_FILESYSTEMWATCHER
    d->fileInfoGatherer.list(indexNode->info.id());
#endif
}

/*!
    \reimp
*/
int DriveModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const DriveModel);

    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return d->driveSystem->root()->visibleChildren.count();
    }

    DriveNodeCSP parentNode = d->node(parent);
    return parentNode->visibleChildren.count();
}

/*!
    \reimp
*/
int DriveModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : DriveModelPrivate::NumColumns;
}

/*!
    \reimp
*/
QVariant DriveModel::data(const QModelIndex &index, int role) const
{
    Q_D(const DriveModel);
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return d->displayName(index);
        case 1: return d->size(index);
        case 2: return d->type(index);
        case 3: return d->time(index);
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0) {
            QIcon icon = d->icon(index);
#ifndef QT_NO_FILESYSTEMWATCHER
            if (icon.isNull()) {
                if (d->node(index)->info.isDir())
                    icon = d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::Folder);
                else
                    icon = d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::File);
            }
#endif // QT_NO_FILESYSTEMWATCHER
            return icon;
        }
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    case FilePermissions:
        {
            int p = permissions(index);
            return p;
        }
    case FilePathRole:
        return "";
    case FileNameRole:
        return d->name(index);
    case NodeRole:
        {
            auto rawNode = static_cast<DriveNode *>(index.internalPointer());
            auto node = d->driveSystem->node(rawNode->info.id());
            return QVariant::fromValue(node);
        }
    }

    return QVariant();
}

/*!
    \internal
*/
QString DriveModelPrivate::size(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    DriveNodeCSP n = node(index);
    if (n->info.isDir()) {
#ifdef Q_OS_MAC
        return QLatin1String("--");
#else
        return QLatin1String("");
#endif
    // Windows   - ""
    // OS X      - "--"
    // Konqueror - "4 KB"
    // Nautilus  - "9 items" (the number of children)
    }
    return size(n->info.size());
}

QString DriveModelPrivate::size(qint64 bytes)
{
    return QLocale::system().formattedDataSize(bytes);
}

/*!
    \internal
*/
QString DriveModelPrivate::time(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
#ifndef QT_NO_DATESTRING
    return node(index)->info.lastModified().toString(Qt::SystemLocaleDate);
#else
    Q_UNUSED(index);
    return QString();
#endif
}

/*
    \internal
*/
QString DriveModelPrivate::type(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    return node(index)->info.mimeType();
}

/*!
    \internal
*/
QString DriveModelPrivate::name(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    DriveNodeSP dirNode = node(index);
    return dirNode->info.fileName();
}

/*!
    \internal
*/
QString DriveModelPrivate::displayName(const QModelIndex &index) const
{
    return name(index);
}

/*!
    \internal
*/
QIcon DriveModelPrivate::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    return node(index)->info.icon();
}

static void displayRenameFailedMessage(const QString &newName)
{
#if QT_CONFIG(messagebox)
    const QString message =
        DriveModel::tr("<b>The name \"%1\" cannot be used.</b>"
                             "<p>Try using another name, with fewer characters or no punctuation marks.")
                             .arg(newName);
    QMessageBox::information(nullptr, DriveModel::tr("Invalid filename"),
                             message, QMessageBox::Ok);
#else
    Q_UNUSED(newName)
#endif // QT_CONFIG(messagebox)
}

/*!
    \reimp
*/
bool DriveModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    Q_D(DriveModel);
    if (!idx.isValid()
        || idx.column() != 0
        || role != Qt::EditRole
        || (flags(idx) & Qt::ItemIsEditable) == 0) {
        return false;
    }

    QString newName = value.toString();
    QString oldName = idx.data().toString();
    if (newName == oldName)
        return true;

    // TODO rename
    return false;
//     const QString parentPath = filePath(parent(idx));

//     if (newName.isEmpty() || QDir::toNativeSeparators(newName).contains(QDir::separator())) {
//         displayRenameFailedMessage(newName);
//         return false;
//     }

//     if (!QDir(parentPath).rename(oldName, newName)) {
//         displayRenameFailedMessage(newName);
//         return false;
//     } else {
//         /*
//             *After re-naming something we don't want the selection to change*
//             - can't remove rows and later insert
//             - can't quickly remove and insert
//             - index pointer can't change because treeview doesn't use persistant index's

//             - if this get any more complicated think of changing it to just
//               use layoutChanged
//          */

//         FileNode *indexNode = d->node(idx);
//         FileNode *parentNode = indexNode->parent;
//         int visibleLocation = parentNode->visibleLocation(parentNode->children.value(indexNode->fileName)->fileName);

//         parentNode->visibleChildren.removeAt(visibleLocation);
//         QScopedPointer<FileNode> nodeToRename(parentNode->children.take(oldName));
//         nodeToRename->fileName = newName;
//         nodeToRename->parent = parentNode;
// #ifndef QT_NO_FILESYSTEMWATCHER
//         nodeToRename->populate(d->fileInfoGatherer.getInfo(DriveFileInfo(&d->driveSystem, parentPath, newName)));
// #endif
//         nodeToRename->isVisible = true;
//         parentNode->children[newName] = nodeToRename.take();
//         parentNode->visibleChildren.insert(visibleLocation, newName);

//         d->delayedSort();
//         emit fileRenamed(parentPath, oldName, newName);
//     }
//     return true;
}

/*!
    \reimp
*/
QVariant DriveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        if (section == 0) {
            // ### TODO oh man this is ugly and doesn't even work all the way!
            // it is still 2 pixels off
            QImage pixmap(16, 1, QImage::Format_Mono);
            pixmap.fill(0);
            pixmap.setAlphaChannel(pixmap.createAlphaMask());
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    QString returnValue;
    switch (section) {
    case 0: returnValue = tr("Name");
            break;
    case 1: returnValue = tr("Size");
            break;
    case 2: returnValue =
#ifdef Q_OS_MAC
                   tr("Kind", "Match OS X Finder");
#else
                   tr("Type", "All other platforms");
#endif
           break;
    // Windows   - Type
    // OS X      - Kind
    // Konqueror - File Type
    // Nautilus  - Type
    case 3: returnValue = tr("Date Modified");
            break;
    default: return QVariant();
    }
    return returnValue;
}

/*!
    \reimp
*/
Qt::ItemFlags DriveModel::flags(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return flags;

    DriveNodeSP indexNode = d->node(index);
    if (d->nameFilterDisables && !d->passNameFilters(indexNode)) {
        flags &= ~Qt::ItemIsEnabled;
        // ### TODO you shouldn't be able to set this as the current item, task 119433
        return flags;
    }

    flags |= Qt::ItemIsDragEnabled;
    if (d->readOnly)
        return flags;
    if ((index.column() == 0) && indexNode->info.permissions() & QFile::WriteUser) {
        flags |= Qt::ItemIsEditable;
        if (indexNode->info.isDir())
            flags |= Qt::ItemIsDropEnabled;
        else
            flags |= Qt::ItemNeverHasChildren;
    }
    return flags;
}

/*!
    \internal
*/
void DriveModelPrivate::_q_performDelayedSort()
{
    Q_Q(DriveModel);
    q->sort(sortColumn, sortOrder);
}


/*
    \internal
    Helper functor used by sort()
*/
class DriveModelSorter
{
public:
    inline DriveModelSorter(int column) : sortColumn(column)
    {
        naturalCompare.setNumericMode(true);
        naturalCompare.setCaseSensitivity(Qt::CaseInsensitive);
    }

    bool compareNodes(DriveNodeCSP ln,
                      DriveNodeCSP rn) const
    {
        const auto & l = ln->info;
        const auto & r = rn->info;
        switch (sortColumn) {
        case 0: {
#ifndef Q_OS_MAC
            // place directories before files
            bool left = l->isDir();
            bool right = r->isDir();
            if (left ^ right)
                return left;
#endif
            return naturalCompare.compare(l.fileName(), r.fileName()) < 0;
                }
        case 1:
        {
            // Directories go first
            bool left = l.isDir();
            bool right = r.isDir();
            if (left ^ right)
                return left;

            qint64 sizeDifference = l.size() - r.size();
            if (sizeDifference == 0)
                return naturalCompare.compare(l.fileName(), r.fileName()) < 0;

            return sizeDifference < 0;
        }
        case 2:
        {
            int compare = naturalCompare.compare(l.mimeType(), r.mimeType());
            if (compare == 0)
                return naturalCompare.compare(l.fileName(), r.fileName()) < 0;

            return compare < 0;
        }
        case 3:
        {
            if (l.lastModified() == r.lastModified())
                return naturalCompare.compare(l.fileName(), r.fileName()) < 0;

            return l.lastModified() < r.lastModified();
        }
        }
        Q_ASSERT(false);
        return false;
    }

    bool operator()(DriveNodeCSP l,
                    DriveNodeCSP r) const
    {
        return compareNodes(l, r);
    }


private:
    QCollator naturalCompare;
    int sortColumn;
};

/*
    \internal

    Sort all of the children of parent
*/
void DriveModelPrivate::sortChildren(int column, const QModelIndex &parent)
{
    Q_Q(DriveModel);

    DriveNodeSP indexNode = node(parent);
    if (indexNode->children.empty()) {
        return;
    }

    std::vector<DriveNodeSP> values;
    for (const auto & p : indexNode->children) {
        if (filtersAcceptsNode(p.second)) {
            values.push_back(p.second);
        } else {
            p.second->isVisible = false;
        }
    }
    DriveModelSorter ms(column);
    std::sort(values.begin(), values.end(), ms);

    // First update the new visible list
    indexNode->visibleChildren.clear();
    //No more dirty item we reset our internal dirty index
    indexNode->dirtyChildrenIndex = -1;
    const int numValues = values.size();
    indexNode->visibleChildren.reserve(numValues);
    for (auto childNode : values) {
        indexNode->visibleChildren.push_back(childNode->info.id());
        childNode->isVisible = true;
    }

    if (!disableRecursiveSort) {
        for (int i = 0; i < q->rowCount(parent); ++i) {
            const QModelIndex childIndex = q->index(i, 0, parent);
            DriveNodeSP indexNode = node(childIndex);
            // Only do a recursive sort on visible nodes
            if (indexNode->isVisible) {
                this->sortChildren(column, childIndex);
            }
        }
    }
}

/*!
    \reimp
*/
void DriveModel::sort(int column, Qt::SortOrder order)
{
    Q_D(DriveModel);
    if (d->sortOrder == order && d->sortColumn == column && !d->forceSort)
        return;

    emit layoutAboutToBeChanged();
    QModelIndexList oldList = persistentIndexList();
    QVector<QPair<DriveNodeSP , int> > oldNodes;
    const int nodeCount = oldList.count();
    oldNodes.reserve(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        const QModelIndex &oldNode = oldList.at(i);
        QPair<DriveNodeSP , int> pair(d->node(oldNode), oldNode.column());
        oldNodes.append(pair);
    }

    if (!(d->sortColumn == column && d->sortOrder != order && !d->forceSort)) {
        //we sort only from where we are, don't need to sort all the model
        d->sortChildren(column, index(rootId()));
        d->sortColumn = column;
        d->forceSort = false;
    }
    d->sortOrder = order;

    QModelIndexList newList;
    const int numOldNodes = oldNodes.size();
    newList.reserve(numOldNodes);
    for (int i = 0; i < numOldNodes; ++i) {
        const QPair<DriveNodeSP , int> &oldNode = oldNodes.at(i);
        newList.append(d->index(oldNode.first, oldNode.second));
    }
    changePersistentIndexList(oldList, newList);
    emit layoutChanged();
}

/*!
    Returns a list of MIME types that can be used to describe a list of items
    in the model.
*/
QStringList DriveModel::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

/*!
    Returns an object that contains a serialized description of the specified
    \a indexes. The format used to describe the items corresponding to the
    indexes is obtained from the mimeTypes() function.

    If the list of indexes is empty, 0 is returned rather than a serialized
    empty list.
*/
QMimeData *DriveModel::mimeData(const QModelIndexList &indexes) const
{
    // TODO
    // QList<QUrl> urls;
    // QList<QModelIndex>::const_iterator it = indexes.begin();
    // for (; it != indexes.end(); ++it)
    //     if ((*it).column() == 0)
    //         urls << QUrl::fromLocalFile(filePath(*it));
    QMimeData *data = new QMimeData();
    // data->setUrls(urls);
    return data;
}

/*!
    Handles the \a data supplied by a drag and drop operation that ended with
    the given \a action over the row in the model specified by the \a row and
    \a column and by the \a parent index.

    \sa supportedDropActions()
*/
bool DriveModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                             int row, int column, const QModelIndex &parent)
{
    Q_D(DriveModel);
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!parent.isValid() || isReadOnly())
        return false;

    bool success = true;
    // QString to = filePath(parent) + QDir::separator();

    QList<QUrl> urls = data->urls();
    QList<QUrl>::const_iterator it = urls.constBegin();

    switch (action) {
    case Qt::CopyAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            // TODO
            success = false;
            // success = QFile::copy(path, to + DriveFileInfo(&d->driveSystem, path).fileName()) && success;
        }
        break;
    case Qt::LinkAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            // TODO
            success = false;
            // success = QFile::link(path, to + DriveFileInfo(&d->driveSystem, path).fileName()) && success;
        }
        break;
    case Qt::MoveAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            // TODO
            success = false;
            // success = QFile::rename(path, to + DriveFileInfo(&d->driveSystem, path).fileName()) && success;
        }
        break;
    default:
        return false;
    }

    return success;
}

/*!
    \reimp
*/
Qt::DropActions DriveModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

/*!
    Returns the path of the item stored in the model under the
    \a index given.
*/
// QString DriveModel::filePath(const QModelIndex &index) const
// {
//     Q_D(const DriveModel);
//     QString fullPath = d->filePath(index);
//     return fullPath;
// }

// QString DriveModelPrivate::filePath(const QModelIndex &index) const
// {
//     Q_Q(const DriveModel);
//     Q_UNUSED(q);
//     if (!index.isValid())
//         return QString();
//     Q_ASSERT(index.model() == q);

//     QStringList path;
//     QModelIndex idx = index;
//     while (idx.isValid()) {
//         DriveNodeSP dirNode = node(idx);
//         if (dirNode) {
//             path.prepend(dirNode->info.fileName());
//         }
//         idx = idx.parent();
//     }
//     QString fullPath = QDir::fromNativeSeparators(path.join(QDir::separator()));
//     if ((fullPath.length() > 2) && fullPath[0] == QLatin1Char('/') && fullPath[1] == QLatin1Char('/'))
//         fullPath = fullPath.mid(1);
//     return fullPath;
// }

/*!
    Create a directory with the \a name in the \a parent model index.
*/
QModelIndex DriveModel::mkdir(const QModelIndex &parent, const QString &name)
{
    Q_D(DriveModel);
    if (!parent.isValid())
        return parent;

    // TODO d->driveSystem.mkdir(parent, name)
    return QModelIndex();
//     QDir dir(filePath(parent));
//     if (!dir.mkdir(name))
//         return QModelIndex();
//     FileNode *parentNode = d->node(parent);
//     d->addNode(parentNode, name, DriveFileInfo());
//     Q_ASSERT(parentNode->children.contains(name));
//     FileNode *node = parentNode->children[name];
// #ifndef QT_NO_FILESYSTEMWATCHER
//     node->populate(d->fileInfoGatherer.getInfo(DriveFileInfo(&d->driveSystem, dir.absolutePath() + QDir::separator() + name)));
// #endif
//     d->addVisibleFiles(parentNode, QStringList(name));
//     return d->index(node);
}

/*!
    Returns the complete OR-ed together combination of QFile::Permission for the \a index.
 */
QFile::Permissions DriveModel::permissions(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    return d->node(index)->info.permissions();
}

/*!
    Sets the directory that is being watched by the model to \a newPath by
    installing a \l{QFileSystemWatcher}{file system watcher} on it. Any
    changes to files and directories within this directory will be
    reflected in the model.

    If the path is changed, the rootPathChanged() signal will be emitted.

    \note This function does not change the structure of the model or
    modify the data available to views. In other words, the "root" of
    the model is \e not changed to include only files and directories
    within the directory specified by \a newPath in the file system.
  */
QModelIndex DriveModel::setRootPath(const QString & host, int port)
{
    Q_D(DriveModel);

    d->setRootPath = true;
    auto rootInfo = d->driveSystem->setBaseUrl(host, port);

    if (!rootInfo.isValid()) {
        return d->index(rootId());
    }

    if (!rootId().isEmpty()) {
        //This line "marks" the node as dirty, so the next fetchMore
        //call on the path will ask the gatherer to install a watcher again
        //But it doesn't re-fetch everything
        d->node(rootId())->populatedChildren = false;
    }

    // We have a new valid root path
    d->rootId = rootInfo.id();
    QModelIndex newRootIndex = d->index(rootId());
    fetchMore(newRootIndex);
    emit rootPathChanged(rootId());
    d->forceSort = true;
    d->delayedSort();
    return newRootIndex;
}

/*!
    The currently set root path

    \sa rootDirectory()
*/
QString DriveModel::rootId() const
{
    Q_D(const DriveModel);
    return d->rootId;
}

/*!
    The currently set directory

    \sa rootPath()
*/
// QDir DriveModel::rootDirectory() const
// {
//     Q_D(const DriveModel);
//     QDir dir(d->rootDir);
//     dir.setNameFilters(nameFilters());
//     dir.setFilter(filter());
//     return dir;
// }

/*!
    Sets the directory model's filter to that specified by \a filters.

    Note that the filter you set should always include the QDir::AllDirs enum value,
    otherwise DriveModel won't be able to read the directory structure.

    \sa QDir::Filters
*/
void DriveModel::setFilter(QDir::Filters filters)
{
    Q_D(DriveModel);
    if (d->filters == filters)
        return;
    d->filters = filters;
    // CaseSensitivity might have changed
    setNameFilters(nameFilters());
    d->forceSort = true;
    d->delayedSort();
}

/*!
    Returns the filter specified for the directory model.

    If a filter has not been set, the default filter is QDir::AllEntries |
    QDir::NoDotAndDotDot | QDir::AllDirs.

    \sa QDir::Filters
*/
QDir::Filters DriveModel::filter() const
{
    Q_D(const DriveModel);
    return d->filters;
}

/*!
    \property DriveModel::readOnly
    \brief Whether the directory model allows writing to the file system

    If this property is set to false, the directory model will allow renaming, copying
    and deleting of files and directories.

    This property is \c true by default
*/
void DriveModel::setReadOnly(bool enable)
{
    Q_D(DriveModel);
    d->readOnly = enable;
}

bool DriveModel::isReadOnly() const
{
    Q_D(const DriveModel);
    return d->readOnly;
}

/*!
    \property DriveModel::nameFilterDisables
    \brief Whether files that don't pass the name filter are hidden or disabled

    This property is \c true by default
*/
void DriveModel::setNameFilterDisables(bool enable)
{
    Q_D(DriveModel);
    if (d->nameFilterDisables == enable)
        return;
    d->nameFilterDisables = enable;
    d->forceSort = true;
    d->delayedSort();
}

bool DriveModel::nameFilterDisables() const
{
    Q_D(const DriveModel);
    return d->nameFilterDisables;
}

/*!
    Sets the name \a filters to apply against the existing files.
*/
void DriveModel::setNameFilters(const QStringList &filters)
{
    // Prep the regexp's ahead of time
#ifndef QT_NO_REGEXP
    Q_D(DriveModel);

    if (!d->bypassFilters.isEmpty()) {
        // update the bypass filter to only bypass the stuff that must be kept around
        d->bypassFilters.clear();
        // We guarantee that rootPath will stick around
        QPersistentModelIndex root(index(rootId()));
        const QModelIndexList persistentList = persistentIndexList();
        for (const auto &persistentIndex : persistentList) {
            DriveNodeSP node = d->node(persistentIndex);
            while (node) {
                if (d->bypassFilters.contains(node.get())) {
                    break;
                }
                if (node->info.isDir()) {
                    d->bypassFilters[node.get()] = true;
                }
                node = node->parent.lock();
            }
        }
    }

    d->nameFilters.clear();
    const Qt::CaseSensitivity caseSensitive =
        (filter() & QDir::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    for (const auto &filter : filters)
        d->nameFilters << QRegExp(filter, caseSensitive, QRegExp::Wildcard);
    d->forceSort = true;
    d->delayedSort();
#endif
}

/*!
    Returns a list of filters applied to the names in the model.
*/
QStringList DriveModel::nameFilters() const
{
    Q_D(const DriveModel);
    QStringList filters;
#ifndef QT_NO_REGEXP
    const int numNameFilters = d->nameFilters.size();
    filters.reserve(numNameFilters);
    for (int i = 0; i < numNameFilters; ++i) {
         filters << d->nameFilters.at(i).pattern();
    }
#endif
    return filters;
}


bool DriveModel::rmdir(const QModelIndex &aindex)
{
    // TODO
    return false;
    // QString path = filePath(aindex);
    // const bool success = QDir().rmdir(path);
#ifndef QT_NO_FILESYSTEMWATCHER
    // if (success) {
        // DriveModelPrivate * d = const_cast<DriveModelPrivate*>(d_func());
        // d->fileInfoGatherer.removePath(path);
    // }
#endif
    // return success;
}

/*!
     \internal

    Performed quick listing and see if any files have been added or removed,
    then fetch more information on visible files.
 */
void DriveModelPrivate::_q_directoryChanged(const QString &directory, const QStringList &files)
{
    DriveNodeSP parentNode = node(directory, false);
    if (parentNode->children.empty()) {
        return;
    }

    QStringList toRemove;
    QStringList newFiles = files;
    std::sort(newFiles.begin(), newFiles.end());
    for (const auto & p : parentNode->children) {
        const auto & fileName = p.second->info.fileName();
        auto it = std::lower_bound(newFiles.begin(), newFiles.end(), fileName);
        if ((it == newFiles.end()) || (fileName < *it)) {
            toRemove.append(p.second->info.id());
        }
    }
    for (const auto & id : toRemove) {
        removeNode(parentNode, id);
    }
}

/*!
    \internal

    Adds a new file to the children of parentNode

    *WARNING* this will change the count of children
*/
DriveNodeSP  DriveModelPrivate::addNode(DriveNodeSP parentNode, const DriveFileInfo& info)
{
    // In the common case, itemLocation == count() so check there first
    return this->driveSystem->upsertNode(info);
}

/*!
    \internal

    File at parentNode->children(itemLocation) has been removed, remove from the lists
    and emit signals if necessary

    *WARNING* this will change the count of children and could change visibleChildren
 */
void DriveModelPrivate::removeNode(DriveNodeSP parentNode, const QString & id)
{
    Q_Q(DriveModel);

    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);

    int vLocation = parentNode->visibleLocation(id);
    if (vLocation >= 0 && !indexHidden) {
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                   translateVisibleLocation(parentNode, vLocation));
    }
    this->driveSystem->deleteNode(id);
    // cleanup sort files after removing rather then re-sorting which is O(n)
    if (vLocation >= 0) {
        parentNode->visibleChildren.removeAt(vLocation);
    }
    if (vLocation >= 0 && !indexHidden) {
        q->endRemoveRows();
    }
}

/*!
    \internal

    File at parentNode->children(itemLocation) was not visible before, but now should be
    and emit signals if necessary.

    *WARNING* this will change the visible count
 */
void DriveModelPrivate::addVisibleFiles(DriveNodeSP parentNode, const QStringList &newFiles)
{
    Q_Q(DriveModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden) {
        q->beginInsertRows(parent, parentNode->visibleChildren.count() , parentNode->visibleChildren.count() + newFiles.count() - 1);
    }

    if (parentNode->dirtyChildrenIndex == -1)
        parentNode->dirtyChildrenIndex = parentNode->visibleChildren.count();

    for (const auto & newId : newFiles) {
        parentNode->visibleChildren.append(newId);
        auto childNode = parentNode->child(newId);
        childNode->isVisible = true;
    }
    if (!indexHidden) {
        q->endInsertRows();
    }
}

/*!
    \internal

    File was visible before, but now should NOT be

    *WARNING* this will change the visible count
 */
void DriveModelPrivate::removeVisibleFile(DriveNodeSP parentNode, int vLocation)
{
    Q_Q(DriveModel);

    if (vLocation == -1) {
        return;
    }

    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden) {
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                   translateVisibleLocation(parentNode, vLocation));
    }
    auto childId = parentNode->visibleChildren.at(vLocation);
    auto childNode = parentNode->child(childId);
    childNode->isVisible = false;
    parentNode->visibleChildren.removeAt(vLocation);
    if (!indexHidden) {
        q->endRemoveRows();
    }
}

/*!
    \internal

    The thread has received new information about files,
    update and emit dataChanged if it has actually changed.
 */
void DriveModelPrivate::_q_fileSystemChanged(const QString & id, const QVector<QPair<QString, DriveFileInfo> > &updates)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_Q(DriveModel);

    std::vector<DriveFileInfo> rowsToUpdate;
    QStringList newFiles;
    DriveNodeSP parentNode = node(id, false);
    QModelIndex parentIndex = index(parentNode);

    for (const auto &update : updates) {
        auto info = update.second;
        auto mit = parentNode->children.find(info.id());
        if (mit == parentNode->children.end()) {
            this->addNode(parentNode, info);
            mit = parentNode->children.find(info.id());
        }
        DriveNodeSP node = mit->second;
        if (node->info != info) {
            continue;
        }

        // FIXME this does not work anymore
        if (true || node->info != info) {
            node->populate(info);
            bypassFilters.remove(node.get());
            // brand new information.
            if (filtersAcceptsNode(node)) {
                if (!node->isVisible) {
                    newFiles.append(info.id());
                } else {
                    rowsToUpdate.push_back(info);
                }
            } else {
                if (node->isVisible) {
                    int visibleLocation = parentNode->visibleLocation(info.id());
                    removeVisibleFile(parentNode, visibleLocation);
                } else {
                    // The file is not visible, don't do anything
                }
            }
        }
    }

    // bundle up all of the changed signals into as few as possible.
    std::sort(rowsToUpdate.begin(), rowsToUpdate.end(), [](const DriveFileInfo & l,
                                                           const DriveFileInfo & r) -> bool {
        return l.fileName() < r.fileName();
    });
    QString min;
    QString max;
    for (const auto & value : rowsToUpdate) {
        max = value.id();
        min = value.id();
        int visibleMin = parentNode->visibleLocation(min);
        int visibleMax = parentNode->visibleLocation(max);
        if (visibleMin >= 0
            && visibleMin < parentNode->visibleChildren.count()
            && parentNode->visibleChildren.at(visibleMin) == min
            && visibleMax >= 0) {
            QModelIndex bottom = q->index(translateVisibleLocation(parentNode, visibleMin), 0, parentIndex);
            QModelIndex top = q->index(translateVisibleLocation(parentNode, visibleMax), 3, parentIndex);
            emit q->dataChanged(bottom, top);
        }
    }

    if (!newFiles.empty()) {
        this->addVisibleFiles(parentNode, newFiles);
    }

    if (newFiles.count() > 0 || (sortColumn != 0 && rowsToUpdate.size() > 0)) {
        forceSort = true;
        delayedSort();
    }
#else
    Q_UNUSED(path)
    Q_UNUSED(updates)
#endif // !QT_NO_FILESYSTEMWATCHER
}

/*!
    \internal
*/
void DriveModelPrivate::_q_resolvedName(const QString &fileName, const QString &resolvedName)
{
    assert(!"deprecated signal call");
}

/*!
    \internal
*/
void DriveModelPrivate::init()
{
    Q_Q(DriveModel);
    qRegisterMetaType<QVector<QPair<QString,DriveFileInfo> > >();
#ifndef QT_NO_FILESYSTEMWATCHER
    this->connect(&fileInfoGatherer, SIGNAL(newListOfFiles(QString,QStringList)),
               SLOT(_q_directoryChanged(QString,QStringList)));
    this->connect(&fileInfoGatherer, SIGNAL(updates(QString,QVector<QPair<QString,DriveFileInfo> >)),
            SLOT(_q_fileSystemChanged(QString,QVector<QPair<QString,DriveFileInfo> >)));
    this->connect(&fileInfoGatherer, SIGNAL(nameResolved(QString,QString)),
            SLOT(_q_resolvedName(QString,QString)));
    q->connect(&fileInfoGatherer, SIGNAL(directoryLoaded(QString)),
               SIGNAL(directoryLoaded(QString)));
#endif // !QT_NO_FILESYSTEMWATCHER
    this->connect(&delayedSortTimer, SIGNAL(timeout()), SLOT(_q_performDelayedSort()), Qt::QueuedConnection);

    // TODO 
    // roleNames.insertMulti(DriveModel::FileIconRole, QByteArrayLiteral("fileIcon")); // == Qt::decoration
    // roleNames.insert(DriveModel::FilePathRole, QByteArrayLiteral("filePath"));
    // roleNames.insert(DriveModel::FileNameRole, QByteArrayLiteral("fileName"));
    // roleNames.insert(DriveModel::FilePermissions, QByteArrayLiteral("filePermissions"));
}

/*!
    \internal

    Returns \c false if node doesn't pass the filters otherwise true

    QDir::Modified is not supported
    QDir::Drives is not supported
*/
bool DriveModelPrivate::filtersAcceptsNode(DriveNodeCSP node) const
{
    // always accept drives
    if (node->parent.lock() == this->driveSystem->root() || bypassFilters.contains(node.get()))
        return true;

    // If we don't know anything yet don't accept it
    if (!node->hasInformation())
        return false;

    const bool filterPermissions = ((filters & QDir::PermissionMask)
                                   && (filters & QDir::PermissionMask) != QDir::PermissionMask);
    const bool hideDirs          = !(filters & (QDir::Dirs | QDir::AllDirs));
    const bool hideFiles         = !(filters & QDir::Files);
    const bool hideReadable      = !(!filterPermissions || (filters & QDir::Readable));
    const bool hideWritable      = !(!filterPermissions || (filters & QDir::Writable));
    const bool hideExecutable    = !(!filterPermissions || (filters & QDir::Executable));
    const bool hideHidden        = !(filters & QDir::Hidden);
    const bool hideSystem        = !(filters & QDir::System);
    const bool hideSymlinks      = (filters & QDir::NoSymLinks);
    const bool hideDot           = (filters & QDir::NoDot);
    const bool hideDotDot        = (filters & QDir::NoDotDot);

    // Note that we match the behavior of entryList and not DriveFileInfo on this.
    const auto & info = node->info;
    bool isDot    = (info.fileName() == QLatin1String("."));
    bool isDotDot = (info.fileName() == QLatin1String(".."));
    if (   (hideHidden && !(isDot || isDotDot) && info.isHidden())
        // || (hideSystem && info.isSystem())
        || (hideDirs && info.isDir())
        || (hideFiles && info.isFile())
        || (hideSymlinks && info.isSymLink())
        // || (hideReadable && info.isReadable())
        // || (hideWritable && info.isWritable())
        // || (hideExecutable && info.isExecutable())
        || (hideDot && isDot)
        || (hideDotDot && isDotDot))
        return false;

    return nameFilterDisables || passNameFilters(node);
}

/*
    \internal

    Returns \c true if node passes the name filters and should be visible.
 */
bool DriveModelPrivate::passNameFilters(DriveNodeCSP node) const
{
#ifndef QT_NO_REGEXP
    if (nameFilters.isEmpty())
        return true;

    // Check the name regularexpression filters
    if (!(node->info.isDir() && (filters & QDir::AllDirs))) {
        for (const auto &nameFilter : nameFilters) {
            QRegExp copy = nameFilter;
            if (copy.exactMatch(node->info.fileName()))
                return true;
        }
        return false;
    }
#endif
    return true;
}

QT_END_NAMESPACE
