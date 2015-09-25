/**
 * @file characterdeviceloader.cpp
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
#include "characterdeviceloader_p.hpp"

#include <QtCore/QEventLoop>

using fbv::loader::CharacterDeviceLoader;

CharacterDeviceLoader::Private::Private( CharacterDeviceLoader * owner ):
QObject(),
owner( owner ),
buffer() {
    this->buffer.open( QIODevice::ReadWrite );
}

void CharacterDeviceLoader::Private::onReadyRead() {
    this->buffer.write( this->owner->getDevice()->readAll() );
}

void CharacterDeviceLoader::Private::onReadFinished() {
    this->buffer.write( this->owner->getDevice()->readAll() );
    this->owner->getDevice()->deleteLater();
    this->buffer.seek( 0 );
    emit this->finished();
}

CharacterDeviceLoader::CharacterDeviceLoader( QIODevice * device ):
AsynchronousLoader( device ),
p_( new Private( this ) ) {
    this->p_->connect( device, SIGNAL( readyRead() ), SLOT( onReadyRead() ) );
    this->p_->connect( device, SIGNAL( readChannelFinished() ), SLOT( onReadFinished() ) );
}

void CharacterDeviceLoader::run() {
    QEventLoop wait;
    wait.connect( this->p_.get(), SIGNAL( finished() ), SLOT( quit() ) );
    wait.exec();

    emit this->finished( this->p_->buffer.data() );
}
