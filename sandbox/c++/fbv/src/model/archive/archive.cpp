/**
 * @file archive.cpp
 * @author Wei-Cheng Pan
 *
 * KomiX, a comics viewer.
 * Copyright (C) 2008  Wei-Cheng Pan <legnaleurc@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "archive.hpp"

#include <QtCore/QCoreApplication>

namespace KomiX {
namespace model {
namespace archive {

// one-shot action
QDir createTmpDir() {
    qsrand( qApp->applicationPid() );
    QString tmpPath( QString( "komix_%1" ).arg( qrand() ) );
    QDir tmpDir( QDir::temp() );
    if( !tmpDir.mkdir( tmpPath ) ) {
        qWarning( "can not make temp dir" );
        // tmpDir will remain to tmp dir
    } else {
        tmpDir.cd( tmpPath );
    }
    return tmpDir;
}

const QDir & getTmpDir() {
    static QDir tmp = createTmpDir();
    return tmp;
}

int delTree( const QDir & dir ) {
    int sum = 0;
    QFileInfoList entry = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs );
    foreach( QFileInfo e, entry ) {
        if( e.isDir() ) {
            sum += delTree( e.absoluteFilePath() );
        } else {
            if( QFile::remove( e.absoluteFilePath() ) ) {
                ++sum;
            }
        }
    }
    dir.rmdir( dir.absolutePath() );
    return sum + 1;
}

}
}
}
