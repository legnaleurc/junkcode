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
#include "drivenode.h"

QT_BEGIN_NAMESPACE

class DriveModelPrivate;


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
    inline bool isHiddenByFilter(DriveNodeSP indexNode, const QModelIndex &index) const
    {
       return (indexNode != this->driveSystem->root() && !index.isValid());
    }
    DriveNodeSP node(const QModelIndex &index) const;
    DriveNodeSP node(const QString &path, bool fetch = true) const;
    inline QModelIndex index(const QString &path, int column = 0) { return index(node(path), column); }
    QModelIndex index(DriveNodeCSP node, int column = 0) const;
    bool filtersAcceptsNode(DriveNodeCSP node) const;
    bool passNameFilters(DriveNodeCSP node) const;
    void removeNode(DriveNodeSP parentNode, const QString &name);
    DriveNodeSP addNode(DriveNodeSP parentNode, const DriveFileInfo &info);
    void addVisibleFiles(DriveNodeSP parentNode, const QStringList &newFiles);
    void removeVisibleFile(DriveNodeSP parentNode, int visibleLocation);
    void sortChildren(int column, const QModelIndex &parent);

    inline int translateVisibleLocation(DriveNodeSP parent, int row) const {
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
    // QString filePath(const QModelIndex &index) const;
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
    QString rootId;
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
    QHash<DriveNodeCSP, bool> bypassFilters;
    bool nameFilterDisables;
    //This flag is an optimization for the QFileDialog
    //It enable a sort which is not recursive, it means
    //we sort only what we see.
    bool disableRecursiveSort;
#ifndef QT_NO_REGEXP
    QList<QRegExp> nameFilters;
#endif

    QBasicTimer fetchingTimer;
    struct Fetching {
        QString dir;
        QString file;
        DriveNodeCSP node;
    };
    QVector<Fetching> toFetch;

};
Q_DECLARE_TYPEINFO(DriveModelPrivate::Fetching, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

#endif
