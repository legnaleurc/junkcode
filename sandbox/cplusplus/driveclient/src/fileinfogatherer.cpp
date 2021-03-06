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

#include "fileinfogatherer_p.h"
#include <QtCore/QtDebug>
#include <QtCore/QDirIterator>


QT_BEGIN_NAMESPACE

/*!
    Creates thread
*/
FileInfoGatherer::FileInfoGatherer(QObject *parent)
    : QThread(parent), abort(false),
#ifndef QT_NO_FILESYSTEMWATCHER
    //   watcher(0),
#endif
      m_iconProvider(&defaultProvider),
      driveSystem(nullptr)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    // watcher = new QFileSystemWatcher(this);
    // connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(list(QString)));
    // connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateFile(QString)));
#endif
    start(LowPriority);
}

/*!
    Destroys thread
*/
FileInfoGatherer::~FileInfoGatherer()
{
    abort.store(true);
    QMutexLocker locker(&mutex);
    condition.wakeAll();
    locker.unlock();
    wait();
}

// void FileInfoGatherer::setIconProvider(QFileIconProvider *provider)
// {
//     m_iconProvider = provider;
// }

QFileIconProvider *FileInfoGatherer::iconProvider() const
{
    return m_iconProvider;
}

/*!
    Fetch extended information for all \a files in \a path

    \sa updateFile(), update(), resolvedName()
*/
void FileInfoGatherer::fetchExtendedInformation(const QString &path, const QStringList &files)
{
    QMutexLocker locker(&mutex);
    // See if we already have this dir/file in our queue
    int loc = this->path.lastIndexOf(path);
    while (loc > 0)  {
        if (this->files.at(loc) == files) {
            return;
        }
        loc = this->path.lastIndexOf(path, loc - 1);
    }
    this->path.push(path);
    this->files.push(files);
    condition.wakeAll();

#ifndef QT_NO_FILESYSTEMWATCHER
    // if (files.isEmpty()
    //     && !path.isEmpty()
    //     && !path.startsWith(QLatin1String("//")) /*don't watch UNC path*/) {
    //     if (!watcher->directories().contains(path))
    //         watcher->addPath(path);
    // }
#endif
}

/*!
    Fetch extended information for all \a filePath

    \sa fetchExtendedInformation()
*/
void FileInfoGatherer::updateFile(const QString &filePath)
{
    QString dir = filePath.mid(0, filePath.lastIndexOf(QLatin1Char('/')));
    QString fileName = filePath.mid(dir.length() + 1);
    fetchExtendedInformation(dir, QStringList(fileName));
}

/*
    List all files in \a directoryPath

    \sa listed()
*/
// void FileInfoGatherer::clear()
// {
// #ifndef QT_NO_FILESYSTEMWATCHER
//     QMutexLocker locker(&mutex);
//     watcher->removePaths(watcher->files());
//     watcher->removePaths(watcher->directories());
// #endif
// }

/*
    Remove a \a path from the watcher

    \sa listed()
*/
// void FileInfoGatherer::removePath(const QString &path)
// {
// #ifndef QT_NO_FILESYSTEMWATCHER
//     QMutexLocker locker(&mutex);
//     watcher->removePath(path);
// #else
//     Q_UNUSED(path);
// #endif
// }

/*
    List all files in \a directoryPath

    \sa listed()
*/
void FileInfoGatherer::list(const QString &directoryPath)
{
    fetchExtendedInformation(directoryPath, QStringList());
}

/*
    Until aborted wait to fetch a directory or files
*/
void FileInfoGatherer::run()
{
    forever {
        QMutexLocker locker(&mutex);
        while (!abort.load() && path.isEmpty())
            condition.wait(&mutex);
        if (abort.load())
            return;
        const QString thisPath = qAsConst(path).front();
        path.pop_front();
        const QStringList thisList = qAsConst(files).front();
        files.pop_front();
        locker.unlock();

        getFileInfos(thisPath, thisList);
    }
}

/*
    Get specific file info's, batch the files so update when we have 100
    items and every 200ms after that
 */
void FileInfoGatherer::getFileInfos(const QString &path, const QStringList &files)
{
    if (path.isEmpty()) {
        qDebug() << "got empty request";
        return;
    }

    QElapsedTimer base;
    base.start();
    DriveFileInfo fileInfo;
    bool firstTime = true;
    QVector<QPair<QString, DriveFileInfo> > updatedFiles;
    QStringList filesToCheck = files;

    QStringList allFiles;
    if (files.isEmpty()) {
        auto children = this->driveSystem->list(path);
        QListIterator<DriveFileInfo> it(children);
        while (!abort.load() && it.hasNext()) {
            const auto & fileInfo = it.next();
            allFiles.append(fileInfo.fileName());
            fetch(fileInfo, base, firstTime, updatedFiles, path);
        }
    }
    if (!allFiles.isEmpty())
        emit newListOfFiles(path, allFiles);

    QStringList::const_iterator filesIt = filesToCheck.constBegin();
    while (!abort.load() && filesIt != filesToCheck.constEnd()) {
        fileInfo = this->driveSystem->info(path + QDir::separator() + *filesIt);
        ++filesIt;
        fetch(fileInfo, base, firstTime, updatedFiles, path);
    }
    if (!updatedFiles.isEmpty())
        emit updates(path, updatedFiles);
    emit directoryLoaded(path);
}

void FileInfoGatherer::fetch(const DriveFileInfo &fileInfo, QElapsedTimer &base, bool &firstTime, QVector<QPair<QString, DriveFileInfo> > &updatedFiles, const QString &path) {
    updatedFiles.append(QPair<QString, DriveFileInfo>(fileInfo.fileName(), fileInfo));
    QElapsedTimer current;
    current.start();
    if ((firstTime && updatedFiles.count() > 100) || base.msecsTo(current) > 1000) {
        emit updates(path, updatedFiles);
        updatedFiles.clear();
        base = current;
        firstTime = false;
    }
}

void FileInfoGatherer::setDriveSystem(DriveSystem * driveSystem) {
    this->driveSystem = driveSystem;
    QObject::connect(driveSystem, &DriveSystem::directoryUpdated, this, &FileInfoGatherer::list);
}

QT_END_NAMESPACE
