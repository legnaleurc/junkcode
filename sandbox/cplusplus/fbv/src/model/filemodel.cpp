/**
 * @file filemodel.cpp
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
#include "filemodel.hpp"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

#include <algorithm>
#include <list>
#include <utility>

namespace {

typedef std::pair< fbv::model::FileModel::KeyFunctor, fbv::model::FileModel::ValueFunctor > FunctorPair;
typedef std::list< FunctorPair > FunctorList;

QMutex * lock() {
    static QMutex m;
    return &m;
}

FunctorList & getFunctorList() {
    static FunctorList fl;
    return fl;
}

} // end of namespace

using fbv::model::FileModel;

std::shared_ptr< FileModel > FileModel::createModel( const QUrl & url ) {
    QMutexLocker locker( ::lock() );
    Q_UNUSED( locker );
    auto it = std::find_if( getFunctorList().begin(), getFunctorList().end(), [&url]( const FunctorPair & that )->bool {
        return that.first( url );
    } );
    if( it == getFunctorList().end() ) {
        return std::shared_ptr< FileModel >();
    } else {
        return it->second( url );
    }
}

bool FileModel::registerModel( const KeyFunctor & key, const ValueFunctor & value ) {
    QMutexLocker locker( ::lock() );
    Q_UNUSED( locker );
    getFunctorList().push_back( std::make_pair( key, value ) );
    return true;
}

void FileModel::initialize() {
    this->doInitialize();
}
