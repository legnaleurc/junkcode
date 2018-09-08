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

#ifndef DRIVEMODEL_P_H
#define DRIVEMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "drivemodel.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QPair>
#include <QtCore/QDir>
#include <QtGui/QIcon>
#include <QtCore/QFileinfo>
#include <QtCore/QTimer>
#include <QtCore/QHash>

#include "drivesystem.h"
#include "fileinfogatherer_p.h"

QT_BEGIN_NAMESPACE

class ExtendedInformation;
class DriveModelPrivate;
class QFileIconProvider;

typedef QString DriveModelNodePathKey;


class FileNode
{
public:
    explicit FileNode(const QString &filename = QString(), FileNode *p = 0)
        : fileName(filename)
        , populatedChildren(false)
        , isVisible(false)
        , dirtyChildrenIndex(-1)
        , parent(p)
        , info(0)
    {}
    ~FileNode() {
        qDeleteAll(children);
        delete info;
        info = 0;
        parent = 0;
    }

    QString fileName;

    inline qint64 size() const { if (info && !info->isDir()) return info->size(); return 0; }
    inline QString type() const { if (info) return info->mimeType(); return QLatin1String(""); }
    inline QDateTime lastModified() const { if (info) return info->lastModified(); return QDateTime(); }
    inline QFile::Permissions permissions() const { if (info) return info->permissions(); return 0; }
    inline bool isReadable() const { return ((permissions() & QFile::ReadUser) != 0); }
    inline bool isWritable() const { return ((permissions() & QFile::WriteUser) != 0); }
    inline bool isExecutable() const { return ((permissions() & QFile::ExeUser) != 0); }
    inline bool isDir() const {
        if (info)
            return info->isDir();
        if (children.count() > 0)
            return true;
        return false;
    }
    inline DriveFileInfo fileInfo() const { if (info) return *info; return DriveFileInfo(); }
    inline bool isFile() const { if (info) return info->isFile(); return true; }
    inline bool isSystem() const { return false; }
    inline bool isHidden() const { if (info) return info->isHidden(); return false; }
    inline bool isSymLink(bool ignoreNtfsSymLinks = false) const { return info && info->isSymLink(); }
    inline bool caseSensitive() const { return true; }
    inline QIcon icon() const { if (info) return info->icon(); return QIcon(); }

    inline bool operator <(const FileNode &node) const {
        if (caseSensitive() || node.caseSensitive())
            return fileName < node.fileName;
        return QString::compare(fileName, node.fileName, Qt::CaseInsensitive) < 0;
    }
    inline bool operator >(const QString &name) const {
        if (caseSensitive())
            return fileName > name;
        return QString::compare(fileName, name, Qt::CaseInsensitive) > 0;
    }
    inline bool operator <(const QString &name) const {
        if (caseSensitive())
            return fileName < name;
        return QString::compare(fileName, name, Qt::CaseInsensitive) < 0;
    }
    bool operator ==(const QString &name) const {
        if (caseSensitive())
            return fileName == name;
        return QString::compare(fileName, name, Qt::CaseInsensitive) == 0;
    }

    inline bool hasInformation() const { return info != 0; }

    void populate(const DriveFileInfo &fileInfo) {
        if (!info)
            info = new DriveFileInfo(fileInfo);
        (*info) = fileInfo;
    }

    // children shouldn't normally be accessed directly, use node()
    inline int visibleLocation(const QString &childName) {
        return visibleChildren.indexOf(childName);
    }

    bool populatedChildren;
    bool isVisible;
    QHash<DriveModelNodePathKey, FileNode *> children;
    QList<QString> visibleChildren;
    int dirtyChildrenIndex;
    FileNode *parent;

    DriveFileInfo *info;
    const DriveSystem * driveSystem;
};


class DriveModelPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(DriveModel)

public:
    enum { NumColumns = 4 };

    explicit DriveModelPrivate(DriveModel *q) :
            q_ptr(q),
            driveSystem(new DriveSystem(this)),
            forceSort(true),
            sortColumn(0),
            sortOrder(Qt::AscendingOrder),
            readOnly(true),
            setRootPath(false),
            filters(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs),
            nameFilterDisables(true), // false on windows, true on mac and unix
            disableRecursiveSort(false)
    {
        delayedSortTimer.setSingleShot(true);
        fileInfoGatherer.setDriveSystem(this->driveSystem);
    }

    void init();
    /*
      \internal

      Return true if index which is owned by node is hidden by the filter.
    */
    inline bool isHiddenByFilter(FileNode *indexNode, const QModelIndex &index) const
    {
       return (indexNode != &root && !index.isValid());
    }
    FileNode *node(const QModelIndex &index) const;
    FileNode *node(const QString &path, bool fetch = true) const;
    inline QModelIndex index(const QString &path, int column = 0) { return index(node(path), column); }
    QModelIndex index(const FileNode *node, int column = 0) const;
    bool filtersAcceptsNode(const FileNode *node) const;
    bool passNameFilters(const FileNode *node) const;
    void removeNode(FileNode *parentNode, const QString &name);
    FileNode* addNode(FileNode *parentNode, const QString &fileName, const DriveFileInfo &info);
    void addVisibleFiles(FileNode *parentNode, const QStringList &newFiles);
    void removeVisibleFile(FileNode *parentNode, int visibleLocation);
    void sortChildren(int column, const QModelIndex &parent);

    inline int translateVisibleLocation(FileNode *parent, int row) const {
        if (sortOrder != Qt::AscendingOrder) {
            if (parent->dirtyChildrenIndex == -1)
                return parent->visibleChildren.count() - row - 1;

            if (row < parent->dirtyChildrenIndex)
                return parent->dirtyChildrenIndex - row - 1;
        }

        return row;
    }

    inline void delayedSort() {
        if (!delayedSortTimer.isActive())
            delayedSortTimer.start(0);
    }

    inline bool indexValid(const QModelIndex &index) const {
         return (index.row() >= 0) && (index.column() >= 0) && (index.model() == q_func());
    }

    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString displayName(const QModelIndex &index) const;
    QString filePath(const QModelIndex &index) const;
    QString size(const QModelIndex &index) const;
    static QString size(qint64 bytes);
    QString type(const QModelIndex &index) const;
    QString time(const QModelIndex &index) const;

public slots:
    void _q_directoryChanged(const QString &directory, const QStringList &list);
    void _q_performDelayedSort();
    void _q_fileSystemChanged(const QString &path, const QVector<QPair<QString, DriveFileInfo> > &);
    void _q_resolvedName(const QString &fileName, const QString &resolvedName);

public:
    static int naturalCompare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs);

    DriveModel * q_ptr;

    DriveSystem * driveSystem;
    QString roodId;
#if QT_CONFIG(filesystemwatcher)
    FileInfoGatherer fileInfoGatherer;
#endif // filesystemwatcher
    QTimer delayedSortTimer;
    bool forceSort;
    int sortColumn;
    Qt::SortOrder sortOrder;
    bool readOnly;
    bool setRootPath;
    QDir::Filters filters;
    QHash<const FileNode*, bool> bypassFilters;
    bool nameFilterDisables;
    //This flag is an optimization for the QFileDialog
    //It enable a sort which is not recursive, it means
    //we sort only what we see.
    bool disableRecursiveSort;
#ifndef QT_NO_REGEXP
    QList<QRegExp> nameFilters;
#endif

    FileNode root;

    QBasicTimer fetchingTimer;
    struct Fetching {
        QString dir;
        QString file;
        const FileNode *node;
    };
    QVector<Fetching> toFetch;

};
Q_DECLARE_TYPEINFO(DriveModelPrivate::Fetching, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

#endif
