/**
 * @file deviceloader.cpp
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
#include "deviceloader_p.hpp"
#include "characterdeviceloader.hpp"
#include "blockdeviceloader.hpp"

#include <QtCore/QBuffer>
#include <QtCore/QThreadPool>
#include <QtGui/QImageReader>

namespace {
/// 2 MiB
const qint64 MAX_DEVICE_SIZE = 10 * 1024 * 1024;
}

using fbv::loader::DeviceLoader;
using fbv::loader::AsynchronousLoader;
using fbv::loader::CharacterDeviceLoader;
using fbv::loader::BlockDeviceLoader;

DeviceLoader::Private::Private( int id, QIODevice * device ):
QObject(),
id( id ),
device( device ) {
}

void DeviceLoader::Private::read( QIODevice * device ) {
    QImageReader iin( device );
    if( iin.supportsAnimation() ) {
        device->seek( 0 );
        QMovie * movie = new QMovie( device );
        device->setParent( movie );
        emit this->finished( this->id, movie );
    } else {
        QPixmap pixmap = QPixmap::fromImageReader( &iin );
        device->deleteLater();
        emit this->finished( this->id, pixmap );
    }
}

void DeviceLoader::Private::onFinished( const QByteArray & data ) {
    QBuffer * buffer = new QBuffer;
    buffer->setData( data );
    buffer->open( QIODevice::ReadOnly );
    this->read( buffer );
    this->device->deleteLater();
}

DeviceLoader::DeviceLoader( int id, QIODevice * device ):
QObject(),
p_( new Private( id, device ) ) {
    this->connect( this->p_.get(), SIGNAL( finished( int, QMovie * ) ), SIGNAL( finished( int, QMovie * ) ) );
    this->connect( this->p_.get(), SIGNAL( finished( int, const QPixmap & ) ), SIGNAL( finished( int, const QPixmap & ) ) );
}

void DeviceLoader::start() const {
    AsynchronousLoader * loader = nullptr;
    if( this->p_->device->isSequential() ) {
        // character device, async operation
        loader = new CharacterDeviceLoader( this->p_->device );
    } else if( this->p_->device->size() >= MAX_DEVICE_SIZE ) {
        // large block device, async operation
        loader = new BlockDeviceLoader( this->p_->device );
    } else {
        // small block device, read directly
        // NOTE
        // somehow QFile will lost file name information while looping QMovie
        // so we must wrap with a QBuffer
        this->p_->onFinished( this->p_->device->readAll() );
        return;
    }
    this->p_->connect( loader, SIGNAL( finished( const QByteArray & ) ), SLOT( onFinished( const QByteArray & ) ) );
    QThreadPool::globalInstance()->start( loader );
}
