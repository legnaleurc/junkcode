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

#ifndef QFILEINFOGATHERER_H
#define QFILEINFOGATHERER_H

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

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QFilesystemWatcher>
#include <QtWidgets/QFileIconProvider>
#include <QtCore/QPair>
#include <QtCore/QStack>
#include <QtCore/QDatetime>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>

#include "drivefileinfo.h"
#include "drivesystem.h"

QT_BEGIN_NAMESPACE

// class ExtendedInformation {
// public:
//     enum Type { Dir, File, System };

//     ExtendedInformation() {}
//     ExtendedInformation(const DriveFileInfo &info) : mFileInfo(info) {}

//     inline bool isDir() { return type() == Dir; }
//     inline bool isFile() { return type() == File; }
//     inline bool isSystem() { return type() == System; }

//     bool operator ==(const ExtendedInformation &fileInfo) const {
//         return mFileInfo == fileInfo.mFileInfo
//             && displayType == fileInfo.displayType
//             && permissions() == fileInfo.permissions()
//             && lastModified() == fileInfo.lastModified();
//     }

//     bool isCaseSensitive() const {
//         return true;
//     }

//     QFile::Permissions permissions() const {
//         return mFileInfo.permissions();
//     }

//     Type type() const {
//         if (mFileInfo.isDir()) {
//             return ExtendedInformation::Dir;
//         }
//         if (mFileInfo.isFile()) {
//             return ExtendedInformation::File;
//         }
//         if (!mFileInfo.exists() && mFileInfo.isSymLink()) {
//             return ExtendedInformation::System;
//         }
//         return ExtendedInformation::System;
//     }

//     bool isSymLink(bool ignoreNtfsSymLinks = false) const
//     {
//         return mFileInfo.isSymLink();
//     }

//     bool isHidden() const {
//         return mFileInfo.isHidden();
//     }

//     DriveFileInfo fileInfo() const {
//         return mFileInfo;
//     }

//     QDateTime lastModified() const {
//         return mFileInfo.lastModified();
//     }

//     qint64 size() const {
//         qint64 size = -1;
//         if (type() == ExtendedInformation::Dir)
//             size = 0;
//         if (type() == ExtendedInformation::File)
//             size = mFileInfo.size();
//         if (!mFileInfo.exists() && !mFileInfo.isSymLink())
//             size = -1;
//         return size;
//     }

//     QString displayType;
//     QIcon icon;

// private :
//     DriveFileInfo mFileInfo;
// };


class FileInfoGatherer : public QThread
{
Q_OBJECT

signals:
    void updates(const QString &directory, const QVector<QPair<QString, DriveFileInfo> > &updates);
    void newListOfFiles(const QString &directory, const QStringList &listOfFiles) const;
    void nameResolved(const QString &fileName, const QString &resolvedName) const;
    void directoryLoaded(const QString &path);

public:
    explicit FileInfoGatherer(QObject *parent = 0);
    ~FileInfoGatherer();

    // only callable from this->thread():
    // void clear();
    // void removePath(const QString &path);
    // ExtendedInformation getInfo(const DriveFileInfo &info) const;
    QFileIconProvider *iconProvider() const;
    bool resolveSymlinks() const;

    void setDriveSystem(DriveSystem * driveSystem);

public slots:
    void list(const QString &directoryPath);
    void fetchExtendedInformation(const QString &path, const QStringList &files);
    void updateFile(const QString &path);
    // void setIconProvider(QFileIconProvider *provider);

private:
    void run() override;
    // called by run():
    void getFileInfos(const QString &path, const QStringList &files);
    void fetch(const DriveFileInfo &info, QElapsedTimer &base, bool &firstTime, QVector<QPair<QString, DriveFileInfo> > &updatedFiles, const QString &path);

private:
    mutable QMutex mutex;
    // begin protected by mutex
    QWaitCondition condition;
    QStack<QString> path;
    QStack<QStringList> files;
    // end protected by mutex
    QAtomicInt abort;

#ifndef QT_NO_FILESYSTEMWATCHER
    // QFileSystemWatcher *watcher;
#endif
    QFileIconProvider *m_iconProvider; // not accessed by run()
    QFileIconProvider defaultProvider;

    DriveSystem * driveSystem;
};

QT_END_NAMESPACE
#endif // QFILEINFOGATHERER_H
