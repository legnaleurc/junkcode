/**
 * @file archivemodel.hpp
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
#ifndef KOMIX_MODEL_ARCHIVE_ARCHIVEMODEL_HPP_
#define KOMIX_MODEL_ARCHIVE_ARCHIVEMODEL_HPP_

#include "archivemodel.hpp"

namespace KomiX {
namespace model {
namespace archive {

class ArchiveModel::Private: public QObject {
    Q_OBJECT
public:
    explicit Private( ArchiveModel * owner, const QFileInfo & root );

    void extract( const QString &, const char * );

public slots:
    void cleanup( int );
    void checkTwo( int );
    void allDone( int );

signals:
    void ready();
    void error( const QString & );

public:
    ArchiveModel * owner;
    QFileInfo root;
    QString hash;
};

}
}
}

#endif
