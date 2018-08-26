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
#include <QtCore/QtDebug>
#if QT_CONFIG(messagebox)
#include <QtWidgets/QMessageBox>
#endif
#include <QtWidgets/QApplication>

#include <algorithm>

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
    \fn QFileInfo DriveModel::fileInfo(const QModelIndex &index) const

    Returns the QFileInfo for the item stored in the model under the given
    \a index.
*/
QFileInfo DriveModel::fileInfo(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    return d->node(index)->fileInfo();
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

    const QString path = d->filePath(aindex);
    const QFileInfo fileInfo(path);
    const bool success = (fileInfo.isFile() || fileInfo.isSymLink())
            ? QFile::remove(path) : QDir(path).removeRecursively();
    return success;
}

/*!
  Constructs a file system model with the given \a parent.
*/
DriveModel::DriveModel(QObject *parent)
    // TODO initialize DriveModelPrivate
    // : QAbstractItemModel(*new DriveModelPrivate, parent)
    : QAbstractItemModel(parent)
{
    Q_D(DriveModel);
    d->init();
}

/*!
    \internal
*/
DriveModel::DriveModel(DriveModelPrivate &dd, QObject *parent)
    // TODO initialize DriveModelPrivate
    // : QAbstractItemModel(dd, parent)
    : QAbstractItemModel(parent)
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
    DriveModelPrivate::QFileSystemNode *parentNode = (d->indexValid(parent) ? d->node(parent) :
                                                   const_cast<DriveModelPrivate::QFileSystemNode*>(&d->root));
    Q_ASSERT(parentNode);

    // now get the internal pointer for the index
    const int i = d->translateVisibleLocation(parentNode, row);
    if (i >= parentNode->visibleChildren.size())
        return QModelIndex();
    const QString &childName = parentNode->visibleChildren.at(i);
    const DriveModelPrivate::QFileSystemNode *indexNode = parentNode->children.value(childName);
    Q_ASSERT(indexNode);

    return createIndex(row, column, const_cast<DriveModelPrivate::QFileSystemNode*>(indexNode));
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
QModelIndex DriveModel::index(const QString &path, int column) const
{
    Q_D(const DriveModel);
    DriveModelPrivate::QFileSystemNode *node = d->node(path, false);
    return d->index(node, column);
}

/*!
    \internal

    Return the QFileSystemNode that goes to index.
  */
DriveModelPrivate::QFileSystemNode *DriveModelPrivate::node(const QModelIndex &index) const
{
    if (!index.isValid())
        return const_cast<QFileSystemNode*>(&root);
    DriveModelPrivate::QFileSystemNode *indexNode = static_cast<DriveModelPrivate::QFileSystemNode*>(index.internalPointer());
    Q_ASSERT(indexNode);
    return indexNode;
}

/*!
    \internal

    Given a path return the matching QFileSystemNode or &root if invalid
*/
DriveModelPrivate::QFileSystemNode *DriveModelPrivate::node(const QString &path, bool fetch) const
{
    Q_Q(const DriveModel);
    Q_UNUSED(q);
    if (path.isEmpty() || path == myComputer() || path.startsWith(QLatin1Char(':')))
        return const_cast<DriveModelPrivate::QFileSystemNode*>(&root);

    // Construct the nodes up to the new root path if they need to be built
    QString absolutePath;
    QString longPath = path;
    if (longPath == rootDir.path())
        absolutePath = rootDir.absolutePath();
    else
        absolutePath = QDir(longPath).absolutePath();

    // ### TODO can we use bool QAbstractFileEngine::caseSensitive() const?
    QStringList pathElements = absolutePath.split(QLatin1Char('/'), QString::SkipEmptyParts);
    if ((pathElements.isEmpty())
        && QDir::fromNativeSeparators(longPath) != QLatin1String("/")
        )
        return const_cast<DriveModelPrivate::QFileSystemNode*>(&root);
    QModelIndex index = QModelIndex(); // start with "My Computer"
    QString elementPath;
    QChar separator = QLatin1Char('/');
    QString trailingSeparator;
    // add the "/" item, since it is a valid path element on Unix
    if (absolutePath[0] == QLatin1Char('/'))
        pathElements.prepend(QLatin1String("/"));

    DriveModelPrivate::QFileSystemNode *parent = node(index);

    for (int i = 0; i < pathElements.count(); ++i) {
        QString element = pathElements.at(i);
        if (i != 0)
            elementPath.append(separator);
        elementPath.append(element);
        if (i == pathElements.count() - 1)
            elementPath.append(trailingSeparator);
        bool alreadyExisted = parent->children.contains(element);

        // we couldn't find the path element, we create a new node since we
        // _know_ that the path is valid
        if (alreadyExisted) {
            if ((parent->children.count() == 0)
                || (parent->caseSensitive()
                    && parent->children.value(element)->fileName != element)
                || (!parent->caseSensitive()
                    && parent->children.value(element)->fileName.toLower() != element.toLower()))
                alreadyExisted = false;
        }

        DriveModelPrivate::QFileSystemNode *node;
        if (!alreadyExisted) {
            // Someone might call ::index("file://cookie/monster/doesn't/like/veggies"),
            // a path that doesn't exists, I.E. don't blindly create directories.
            QFileInfo info(elementPath);
            if (!info.exists())
                return const_cast<DriveModelPrivate::QFileSystemNode*>(&root);
            DriveModelPrivate *p = const_cast<DriveModelPrivate*>(this);
            node = p->addNode(parent, element,info);
#ifndef QT_NO_FILESYSTEMWATCHER
            node->populate(fileInfoGatherer.getInfo(info));
#endif
        } else {
            node = parent->children.value(element);
        }

        Q_ASSERT(node);
        if (!node->isVisible) {
            // It has been filtered out
            if (alreadyExisted && node->hasInformation() && !fetch)
                return const_cast<DriveModelPrivate::QFileSystemNode*>(&root);

            DriveModelPrivate *p = const_cast<DriveModelPrivate*>(this);
            p->addVisibleFiles(parent, QStringList(element));
            if (!p->bypassFilters.contains(node))
                p->bypassFilters[node] = 1;
            QString dir = q->filePath(this->index(parent));
            if (!node->hasInformation() && fetch) {
                Fetching f = { std::move(dir), std::move(element), node };
                p->toFetch.append(std::move(f));
                p->fetchingTimer.start(0, const_cast<DriveModel*>(q));
            }
        }
        parent = node;
    }

    return parent;
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
            const DriveModelPrivate::QFileSystemNode *node = d->toFetch.at(i).node;
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
    DriveModelPrivate::QFileSystemNode *n = d->node(index);
    if (n->hasInformation())
        return n->isDir();
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
    return d->node(index)->size();
}

/*!
    Returns the type of file \a index such as "Directory" or "JPEG file".
  */
QString DriveModel::type(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!index.isValid())
        return QString();
    return d->node(index)->type();
}

/*!
    Returns the date and time when \a index was last modified.
 */
QDateTime DriveModel::lastModified(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!index.isValid())
        return QDateTime();
    return d->node(index)->lastModified();
}

/*!
    \reimp
*/
QModelIndex DriveModel::parent(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    if (!d->indexValid(index))
        return QModelIndex();

    DriveModelPrivate::QFileSystemNode *indexNode = d->node(index);
    Q_ASSERT(indexNode != 0);
    DriveModelPrivate::QFileSystemNode *parentNode = indexNode->parent;
    if (parentNode == 0 || parentNode == &d->root)
        return QModelIndex();

    // get the parent's row
    DriveModelPrivate::QFileSystemNode *grandParentNode = parentNode->parent;
    Q_ASSERT(grandParentNode->children.contains(parentNode->fileName));
    int visualRow = d->translateVisibleLocation(grandParentNode, grandParentNode->visibleLocation(grandParentNode->children.value(parentNode->fileName)->fileName));
    if (visualRow == -1)
        return QModelIndex();
    return createIndex(visualRow, 0, parentNode);
}

/*
    \internal

    return the index for node
*/
QModelIndex DriveModelPrivate::index(const DriveModelPrivate::QFileSystemNode *node, int column) const
{
    Q_Q(const DriveModel);
    DriveModelPrivate::QFileSystemNode *parentNode = (node ? node->parent : 0);
    if (node == &root || !parentNode)
        return QModelIndex();

    // get the parent's row
    Q_ASSERT(node);
    if (!node->isVisible)
        return QModelIndex();

    int visualRow = translateVisibleLocation(parentNode, parentNode->visibleLocation(node->fileName));
    return q->createIndex(visualRow, column, const_cast<QFileSystemNode*>(node));
}

/*!
    \reimp
*/
bool DriveModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const DriveModel);
    if (parent.column() > 0)
        return false;

    if (!parent.isValid()) // drives
        return true;

    const DriveModelPrivate::QFileSystemNode *indexNode = d->node(parent);
    Q_ASSERT(indexNode);
    return (indexNode->isDir());
}

/*!
    \reimp
 */
bool DriveModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const DriveModel);
    const DriveModelPrivate::QFileSystemNode *indexNode = d->node(parent);
    return (!indexNode->populatedChildren);
}

/*!
    \reimp
 */
void DriveModel::fetchMore(const QModelIndex &parent)
{
    Q_D(DriveModel);
    if (!d->setRootPath)
        return;
    DriveModelPrivate::QFileSystemNode *indexNode = d->node(parent);
    if (indexNode->populatedChildren)
        return;
    indexNode->populatedChildren = true;
#ifndef QT_NO_FILESYSTEMWATCHER
    d->fileInfoGatherer.list(filePath(parent));
#endif
}

/*!
    \reimp
*/
int DriveModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const DriveModel);
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return d->root.visibleChildren.count();

    const DriveModelPrivate::QFileSystemNode *parentNode = d->node(parent);
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
    Returns the data stored under the given \a role for the item "My Computer".

    \sa Qt::ItemDataRole
 */
QVariant DriveModel::myComputer(int role) const
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_D(const DriveModel);
#endif
    switch (role) {
    case Qt::DisplayRole:
        return DriveModelPrivate::myComputer();
#ifndef QT_NO_FILESYSTEMWATCHER
    case Qt::DecorationRole:
        return d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::Computer);
#endif
    }
    return QVariant();
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
    case FilePathRole:
        return filePath(index);
    case FileNameRole:
        return d->name(index);
    case Qt::DecorationRole:
        if (index.column() == 0) {
            QIcon icon = d->icon(index);
#ifndef QT_NO_FILESYSTEMWATCHER
            if (icon.isNull()) {
                if (d->node(index)->isDir())
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
        int p = permissions(index);
        return p;
    }

    return QVariant();
}

/*!
    \internal
*/
QString DriveModelPrivate::size(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    const QFileSystemNode *n = node(index);
    if (n->isDir()) {
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
    return size(n->size());
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
    return node(index)->lastModified().toString(Qt::SystemLocaleDate);
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
    return node(index)->type();
}

/*!
    \internal
*/
QString DriveModelPrivate::name(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    QFileSystemNode *dirNode = node(index);
    if (
#ifndef QT_NO_FILESYSTEMWATCHER
        fileInfoGatherer.resolveSymlinks() &&
#endif
        !resolvedSymLinks.isEmpty() && dirNode->isSymLink(/* ignoreNtfsSymLinks = */ true)) {
        QString fullPath = QDir::fromNativeSeparators(filePath(index));
        return resolvedSymLinks.value(fullPath, dirNode->fileName);
    }
    return dirNode->fileName;
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
    return node(index)->icon();
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

    const QString parentPath = filePath(parent(idx));

    if (newName.isEmpty() || QDir::toNativeSeparators(newName).contains(QDir::separator())) {
        displayRenameFailedMessage(newName);
        return false;
    }

    if (!QDir(parentPath).rename(oldName, newName)) {
        displayRenameFailedMessage(newName);
        return false;
    } else {
        /*
            *After re-naming something we don't want the selection to change*
            - can't remove rows and later insert
            - can't quickly remove and insert
            - index pointer can't change because treeview doesn't use persistant index's

            - if this get any more complicated think of changing it to just
              use layoutChanged
         */

        DriveModelPrivate::QFileSystemNode *indexNode = d->node(idx);
        DriveModelPrivate::QFileSystemNode *parentNode = indexNode->parent;
        int visibleLocation = parentNode->visibleLocation(parentNode->children.value(indexNode->fileName)->fileName);

        parentNode->visibleChildren.removeAt(visibleLocation);
        QScopedPointer<DriveModelPrivate::QFileSystemNode> nodeToRename(parentNode->children.take(oldName));
        nodeToRename->fileName = newName;
        nodeToRename->parent = parentNode;
#ifndef QT_NO_FILESYSTEMWATCHER
        nodeToRename->populate(d->fileInfoGatherer.getInfo(QFileInfo(parentPath, newName)));
#endif
        nodeToRename->isVisible = true;
        parentNode->children[newName] = nodeToRename.take();
        parentNode->visibleChildren.insert(visibleLocation, newName);

        d->delayedSort();
        emit fileRenamed(parentPath, oldName, newName);
    }
    return true;
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

    DriveModelPrivate::QFileSystemNode *indexNode = d->node(index);
    if (d->nameFilterDisables && !d->passNameFilters(indexNode)) {
        flags &= ~Qt::ItemIsEnabled;
        // ### TODO you shouldn't be able to set this as the current item, task 119433
        return flags;
    }

    flags |= Qt::ItemIsDragEnabled;
    if (d->readOnly)
        return flags;
    if ((index.column() == 0) && indexNode->permissions() & QFile::WriteUser) {
        flags |= Qt::ItemIsEditable;
        if (indexNode->isDir())
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

    bool compareNodes(const DriveModelPrivate::QFileSystemNode *l,
                    const DriveModelPrivate::QFileSystemNode *r) const
    {
        switch (sortColumn) {
        case 0: {
#ifndef Q_OS_MAC
            // place directories before files
            bool left = l->isDir();
            bool right = r->isDir();
            if (left ^ right)
                return left;
#endif
            return naturalCompare.compare(l->fileName, r->fileName) < 0;
                }
        case 1:
        {
            // Directories go first
            bool left = l->isDir();
            bool right = r->isDir();
            if (left ^ right)
                return left;

            qint64 sizeDifference = l->size() - r->size();
            if (sizeDifference == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return sizeDifference < 0;
        }
        case 2:
        {
            int compare = naturalCompare.compare(l->type(), r->type());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        case 3:
        {
            if (l->lastModified() == r->lastModified())
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return l->lastModified() < r->lastModified();
        }
        }
        Q_ASSERT(false);
        return false;
    }

    bool operator()(const DriveModelPrivate::QFileSystemNode *l,
                    const DriveModelPrivate::QFileSystemNode *r) const
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
    DriveModelPrivate::QFileSystemNode *indexNode = node(parent);
    if (indexNode->children.count() == 0)
        return;

    QVector<DriveModelPrivate::QFileSystemNode*> values;

    for (auto iterator = indexNode->children.constBegin(), cend = indexNode->children.constEnd(); iterator != cend; ++iterator) {
        if (filtersAcceptsNode(iterator.value())) {
            values.append(iterator.value());
        } else {
            iterator.value()->isVisible = false;
        }
    }
    DriveModelSorter ms(column);
    std::sort(values.begin(), values.end(), ms);
    // First update the new visible list
    indexNode->visibleChildren.clear();
    //No more dirty item we reset our internal dirty index
    indexNode->dirtyChildrenIndex = -1;
    const int numValues = values.count();
    indexNode->visibleChildren.reserve(numValues);
    for (int i = 0; i < numValues; ++i) {
        indexNode->visibleChildren.append(values.at(i)->fileName);
        values.at(i)->isVisible = true;
    }

    if (!disableRecursiveSort) {
        for (int i = 0; i < q->rowCount(parent); ++i) {
            const QModelIndex childIndex = q->index(i, 0, parent);
            DriveModelPrivate::QFileSystemNode *indexNode = node(childIndex);
            //Only do a recursive sort on visible nodes
            if (indexNode->isVisible)
                sortChildren(column, childIndex);
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
    QVector<QPair<DriveModelPrivate::QFileSystemNode*, int> > oldNodes;
    const int nodeCount = oldList.count();
    oldNodes.reserve(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        const QModelIndex &oldNode = oldList.at(i);
        QPair<DriveModelPrivate::QFileSystemNode*, int> pair(d->node(oldNode), oldNode.column());
        oldNodes.append(pair);
    }

    if (!(d->sortColumn == column && d->sortOrder != order && !d->forceSort)) {
        //we sort only from where we are, don't need to sort all the model
        d->sortChildren(column, index(rootPath()));
        d->sortColumn = column;
        d->forceSort = false;
    }
    d->sortOrder = order;

    QModelIndexList newList;
    const int numOldNodes = oldNodes.size();
    newList.reserve(numOldNodes);
    for (int i = 0; i < numOldNodes; ++i) {
        const QPair<DriveModelPrivate::QFileSystemNode*, int> &oldNode = oldNodes.at(i);
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
    QList<QUrl> urls;
    QList<QModelIndex>::const_iterator it = indexes.begin();
    for (; it != indexes.end(); ++it)
        if ((*it).column() == 0)
            urls << QUrl::fromLocalFile(filePath(*it));
    QMimeData *data = new QMimeData();
    data->setUrls(urls);
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
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!parent.isValid() || isReadOnly())
        return false;

    bool success = true;
    QString to = filePath(parent) + QDir::separator();

    QList<QUrl> urls = data->urls();
    QList<QUrl>::const_iterator it = urls.constBegin();

    switch (action) {
    case Qt::CopyAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::copy(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    case Qt::LinkAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::link(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    case Qt::MoveAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::rename(path, to + QFileInfo(path).fileName()) && success;
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
QString DriveModel::filePath(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    QString fullPath = d->filePath(index);
    DriveModelPrivate::QFileSystemNode *dirNode = d->node(index);
    if (dirNode->isSymLink()
#ifndef QT_NO_FILESYSTEMWATCHER
        && d->fileInfoGatherer.resolveSymlinks()
#endif
        && d->resolvedSymLinks.contains(fullPath)
        && dirNode->isDir()) {
        QFileInfo resolvedInfo(fullPath);
        resolvedInfo = resolvedInfo.canonicalFilePath();
        if (resolvedInfo.exists())
            return resolvedInfo.filePath();
    }
    return fullPath;
}

QString DriveModelPrivate::filePath(const QModelIndex &index) const
{
    Q_Q(const DriveModel);
    Q_UNUSED(q);
    if (!index.isValid())
        return QString();
    Q_ASSERT(index.model() == q);

    QStringList path;
    QModelIndex idx = index;
    while (idx.isValid()) {
        DriveModelPrivate::QFileSystemNode *dirNode = node(idx);
        if (dirNode)
            path.prepend(dirNode->fileName);
        idx = idx.parent();
    }
    QString fullPath = QDir::fromNativeSeparators(path.join(QDir::separator()));
#if !defined(Q_OS_WIN)
    if ((fullPath.length() > 2) && fullPath[0] == QLatin1Char('/') && fullPath[1] == QLatin1Char('/'))
        fullPath = fullPath.mid(1);
#else
    if (fullPath.length() == 2 && fullPath.endsWith(QLatin1Char(':')))
        fullPath.append(QLatin1Char('/'));
#endif
    return fullPath;
}

/*!
    Create a directory with the \a name in the \a parent model index.
*/
QModelIndex DriveModel::mkdir(const QModelIndex &parent, const QString &name)
{
    Q_D(DriveModel);
    if (!parent.isValid())
        return parent;

    QDir dir(filePath(parent));
    if (!dir.mkdir(name))
        return QModelIndex();
    DriveModelPrivate::QFileSystemNode *parentNode = d->node(parent);
    d->addNode(parentNode, name, QFileInfo());
    Q_ASSERT(parentNode->children.contains(name));
    DriveModelPrivate::QFileSystemNode *node = parentNode->children[name];
#ifndef QT_NO_FILESYSTEMWATCHER
    node->populate(d->fileInfoGatherer.getInfo(QFileInfo(dir.absolutePath() + QDir::separator() + name)));
#endif
    d->addVisibleFiles(parentNode, QStringList(name));
    return d->index(node);
}

/*!
    Returns the complete OR-ed together combination of QFile::Permission for the \a index.
 */
QFile::Permissions DriveModel::permissions(const QModelIndex &index) const
{
    Q_D(const DriveModel);
    return d->node(index)->permissions();
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
QModelIndex DriveModel::setRootPath(const QString &newPath)
{
    Q_D(DriveModel);
    QString longNewPath = newPath;
    QDir newPathDir(longNewPath);
    //we remove .. and . from the given path if exist
    if (!newPath.isEmpty()) {
        longNewPath = QDir::cleanPath(longNewPath);
        newPathDir.setPath(longNewPath);
    }

    d->setRootPath = true;

    //user don't ask for the root path ("") but the conversion failed
    if (!newPath.isEmpty() && longNewPath.isEmpty())
        return d->index(rootPath());

    if (d->rootDir.path() == longNewPath)
        return d->index(rootPath());

    bool showDrives = (longNewPath.isEmpty() || longNewPath == d->myComputer());
    if (!showDrives && !newPathDir.exists())
        return d->index(rootPath());

    //We remove the watcher on the previous path
    if (!rootPath().isEmpty() && rootPath() != QLatin1String(".")) {
        //This remove the watcher for the old rootPath
#ifndef QT_NO_FILESYSTEMWATCHER
        d->fileInfoGatherer.removePath(rootPath());
#endif
        //This line "marks" the node as dirty, so the next fetchMore
        //call on the path will ask the gatherer to install a watcher again
        //But it doesn't re-fetch everything
        d->node(rootPath())->populatedChildren = false;
    }

    // We have a new valid root path
    d->rootDir = newPathDir;
    QModelIndex newRootIndex;
    if (showDrives) {
        // otherwise dir will become '.'
        d->rootDir.setPath(QLatin1String(""));
    } else {
        newRootIndex = d->index(newPathDir.path());
    }
    fetchMore(newRootIndex);
    emit rootPathChanged(longNewPath);
    d->forceSort = true;
    d->delayedSort();
    return newRootIndex;
}

/*!
    The currently set root path

    \sa rootDirectory()
*/
QString DriveModel::rootPath() const
{
    Q_D(const DriveModel);
    return d->rootDir.path();
}

/*!
    The currently set directory

    \sa rootPath()
*/
QDir DriveModel::rootDirectory() const
{
    Q_D(const DriveModel);
    QDir dir(d->rootDir);
    dir.setNameFilters(nameFilters());
    dir.setFilter(filter());
    return dir;
}

/*!
    Sets the \a provider of file icons for the directory model.
*/
void DriveModel::setIconProvider(QFileIconProvider *provider)
{
    Q_D(DriveModel);
#ifndef QT_NO_FILESYSTEMWATCHER
    d->fileInfoGatherer.setIconProvider(provider);
#endif
    d->root.updateIcon(provider, QString());
}

/*!
    Returns the file icon provider for this directory model.
*/
QFileIconProvider *DriveModel::iconProvider() const
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_D(const DriveModel);
    return d->fileInfoGatherer.iconProvider();
#else
    return 0;
#endif
}

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
    \property DriveModel::resolveSymlinks
    \brief Whether the directory model should resolve symbolic links

    This is only relevant on Windows.

    By default, this property is \c true.
*/
void DriveModel::setResolveSymlinks(bool enable)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_D(DriveModel);
    d->fileInfoGatherer.setResolveSymlinks(enable);
#else
    Q_UNUSED(enable)
#endif
}

bool DriveModel::resolveSymlinks() const
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_D(const DriveModel);
    return d->fileInfoGatherer.resolveSymlinks();
#else
    return false;
#endif
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
        QPersistentModelIndex root(index(rootPath()));
        const QModelIndexList persistentList = persistentIndexList();
        for (const auto &persistentIndex : persistentList) {
            DriveModelPrivate::QFileSystemNode *node = d->node(persistentIndex);
            while (node) {
                if (d->bypassFilters.contains(node))
                    break;
                if (node->isDir())
                    d->bypassFilters[node] = true;
                node = node->parent;
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

/*!
    \reimp
*/
bool DriveModel::event(QEvent *event)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_D(DriveModel);
    if (event->type() == QEvent::LanguageChange) {
        d->root.retranslateStrings(d->fileInfoGatherer.iconProvider(), QString());
        return true;
    }
#endif
    return QAbstractItemModel::event(event);
}

bool DriveModel::rmdir(const QModelIndex &aindex)
{
    QString path = filePath(aindex);
    const bool success = QDir().rmdir(path);
#ifndef QT_NO_FILESYSTEMWATCHER
    if (success) {
        DriveModelPrivate * d = const_cast<DriveModelPrivate*>(d_func());
        d->fileInfoGatherer.removePath(path);
    }
#endif
    return success;
}

/*!
     \internal

    Performed quick listing and see if any files have been added or removed,
    then fetch more information on visible files.
 */
void DriveModelPrivate::_q_directoryChanged(const QString &directory, const QStringList &files)
{
    DriveModelPrivate::QFileSystemNode *parentNode = node(directory, false);
    if (parentNode->children.count() == 0)
        return;
    QStringList toRemove;
    QStringList newFiles = files;
    std::sort(newFiles.begin(), newFiles.end());
    for (auto i = parentNode->children.constBegin(), cend = parentNode->children.constEnd(); i != cend; ++i) {
        QStringList::iterator iterator = std::lower_bound(newFiles.begin(), newFiles.end(), i.value()->fileName);
        if ((iterator == newFiles.end()) || (i.value()->fileName < *iterator))
            toRemove.append(i.value()->fileName);
    }
    for (int i = 0 ; i < toRemove.count() ; ++i )
        removeNode(parentNode, toRemove[i]);
}

/*!
    \internal

    Adds a new file to the children of parentNode

    *WARNING* this will change the count of children
*/
DriveModelPrivate::QFileSystemNode* DriveModelPrivate::addNode(QFileSystemNode *parentNode, const QString &fileName, const QFileInfo& info)
{
    // In the common case, itemLocation == count() so check there first
    DriveModelPrivate::QFileSystemNode *node = new DriveModelPrivate::QFileSystemNode(fileName, parentNode);
#ifndef QT_NO_FILESYSTEMWATCHER
    node->populate(info);
#else
    Q_UNUSED(info)
#endif
    Q_ASSERT(!parentNode->children.contains(fileName));
    parentNode->children.insert(fileName, node);
    return node;
}

/*!
    \internal

    File at parentNode->children(itemLocation) has been removed, remove from the lists
    and emit signals if necessary

    *WARNING* this will change the count of children and could change visibleChildren
 */
void DriveModelPrivate::removeNode(DriveModelPrivate::QFileSystemNode *parentNode, const QString& name)
{
    Q_Q(DriveModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);

    int vLocation = parentNode->visibleLocation(name);
    if (vLocation >= 0 && !indexHidden)
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                       translateVisibleLocation(parentNode, vLocation));
    QFileSystemNode * node = parentNode->children.take(name);
    delete node;
    // cleanup sort files after removing rather then re-sorting which is O(n)
    if (vLocation >= 0)
        parentNode->visibleChildren.removeAt(vLocation);
    if (vLocation >= 0 && !indexHidden)
        q->endRemoveRows();
}

/*!
    \internal

    File at parentNode->children(itemLocation) was not visible before, but now should be
    and emit signals if necessary.

    *WARNING* this will change the visible count
 */
void DriveModelPrivate::addVisibleFiles(QFileSystemNode *parentNode, const QStringList &newFiles)
{
    Q_Q(DriveModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden) {
        q->beginInsertRows(parent, parentNode->visibleChildren.count() , parentNode->visibleChildren.count() + newFiles.count() - 1);
    }

    if (parentNode->dirtyChildrenIndex == -1)
        parentNode->dirtyChildrenIndex = parentNode->visibleChildren.count();

    for (const auto &newFile : newFiles) {
        parentNode->visibleChildren.append(newFile);
        parentNode->children.value(newFile)->isVisible = true;
    }
    if (!indexHidden)
      q->endInsertRows();
}

/*!
    \internal

    File was visible before, but now should NOT be

    *WARNING* this will change the visible count
 */
void DriveModelPrivate::removeVisibleFile(QFileSystemNode *parentNode, int vLocation)
{
    Q_Q(DriveModel);
    if (vLocation == -1)
        return;
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden)
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                       translateVisibleLocation(parentNode, vLocation));
    parentNode->children.value(parentNode->visibleChildren.at(vLocation))->isVisible = false;
    parentNode->visibleChildren.removeAt(vLocation);
    if (!indexHidden)
        q->endRemoveRows();
}

/*!
    \internal

    The thread has received new information about files,
    update and emit dataChanged if it has actually changed.
 */
void DriveModelPrivate::_q_fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &updates)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    Q_Q(DriveModel);
    QVector<QString> rowsToUpdate;
    QStringList newFiles;
    DriveModelPrivate::QFileSystemNode *parentNode = node(path, false);
    QModelIndex parentIndex = index(parentNode);
    for (const auto &update : updates) {
        QString fileName = update.first;
        Q_ASSERT(!fileName.isEmpty());
        QExtendedInformation info = fileInfoGatherer.getInfo(update.second);
        bool previouslyHere = parentNode->children.contains(fileName);
        if (!previouslyHere) {
            addNode(parentNode, fileName, info.fileInfo());
        }
        DriveModelPrivate::QFileSystemNode * node = parentNode->children.value(fileName);
        bool isCaseSensitive = parentNode->caseSensitive();
        if (isCaseSensitive) {
            if (node->fileName != fileName)
                continue;
        } else {
            if (QString::compare(node->fileName,fileName,Qt::CaseInsensitive) != 0)
                continue;
        }
        if (isCaseSensitive) {
            Q_ASSERT(node->fileName == fileName);
        } else {
            node->fileName = fileName;
        }

        if (*node != info ) {
            node->populate(info);
            bypassFilters.remove(node);
            // brand new information.
            if (filtersAcceptsNode(node)) {
                if (!node->isVisible) {
                    newFiles.append(fileName);
                } else {
                    rowsToUpdate.append(fileName);
                }
            } else {
                if (node->isVisible) {
                    int visibleLocation = parentNode->visibleLocation(fileName);
                    removeVisibleFile(parentNode, visibleLocation);
                } else {
                    // The file is not visible, don't do anything
                }
            }
        }
    }

    // bundle up all of the changed signals into as few as possible.
    std::sort(rowsToUpdate.begin(), rowsToUpdate.end());
    QString min;
    QString max;
    for (int i = 0; i < rowsToUpdate.count(); ++i) {
        QString value = rowsToUpdate.at(i);
        //##TODO is there a way to bundle signals with QString as the content of the list?
        /*if (min.isEmpty()) {
            min = value;
            if (i != rowsToUpdate.count() - 1)
                continue;
        }
        if (i != rowsToUpdate.count() - 1) {
            if ((value == min + 1 && max.isEmpty()) || value == max + 1) {
                max = value;
                continue;
            }
        }*/
        max = value;
        min = value;
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

        /*min = QString();
        max = QString();*/
    }

    if (newFiles.count() > 0) {
        addVisibleFiles(parentNode, newFiles);
    }

    if (newFiles.count() > 0 || (sortColumn != 0 && rowsToUpdate.count() > 0)) {
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
    resolvedSymLinks[fileName] = resolvedName;
}

/*!
    \internal
*/
void DriveModelPrivate::init()
{
    Q_Q(DriveModel);
    qRegisterMetaType<QVector<QPair<QString,QFileInfo> > >();
#ifndef QT_NO_FILESYSTEMWATCHER
    q->connect(&fileInfoGatherer, SIGNAL(newListOfFiles(QString,QStringList)),
               q, SLOT(_q_directoryChanged(QString,QStringList)));
    q->connect(&fileInfoGatherer, SIGNAL(updates(QString,QVector<QPair<QString,QFileInfo> >)),
            q, SLOT(_q_fileSystemChanged(QString,QVector<QPair<QString,QFileInfo> >)));
    q->connect(&fileInfoGatherer, SIGNAL(nameResolved(QString,QString)),
            q, SLOT(_q_resolvedName(QString,QString)));
    q->connect(&fileInfoGatherer, SIGNAL(directoryLoaded(QString)),
               q, SIGNAL(directoryLoaded(QString)));
#endif // !QT_NO_FILESYSTEMWATCHER
    q->connect(&delayedSortTimer, SIGNAL(timeout()), q, SLOT(_q_performDelayedSort()), Qt::QueuedConnection);

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
bool DriveModelPrivate::filtersAcceptsNode(const QFileSystemNode *node) const
{
    // always accept drives
    if (node->parent == &root || bypassFilters.contains(node))
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

    // Note that we match the behavior of entryList and not QFileInfo on this.
    bool isDot    = (node->fileName == QLatin1String("."));
    bool isDotDot = (node->fileName == QLatin1String(".."));
    if (   (hideHidden && !(isDot || isDotDot) && node->isHidden())
        || (hideSystem && node->isSystem())
        || (hideDirs && node->isDir())
        || (hideFiles && node->isFile())
        || (hideSymlinks && node->isSymLink())
        || (hideReadable && node->isReadable())
        || (hideWritable && node->isWritable())
        || (hideExecutable && node->isExecutable())
        || (hideDot && isDot)
        || (hideDotDot && isDotDot))
        return false;

    return nameFilterDisables || passNameFilters(node);
}

/*
    \internal

    Returns \c true if node passes the name filters and should be visible.
 */
bool DriveModelPrivate::passNameFilters(const QFileSystemNode *node) const
{
#ifndef QT_NO_REGEXP
    if (nameFilters.isEmpty())
        return true;

    // Check the name regularexpression filters
    if (!(node->isDir() && (filters & QDir::AllDirs))) {
        for (const auto &nameFilter : nameFilters) {
            QRegExp copy = nameFilter;
            if (copy.exactMatch(node->fileName))
                return true;
        }
        return false;
    }
#endif
    return true;
}

QT_END_NAMESPACE