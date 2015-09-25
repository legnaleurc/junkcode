/**
 * @file directoryhook.cpp
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
#include "directoryhook.hpp"
#include "global.hpp"

#include <QtCore/QUrl>
#include <QtWidgets/QFileDialog>

namespace {

QAction * hookHelper( QWidget * parent ) {
    return new KomiX::model::directory::DirectoryHook( parent );
}

const bool registered = KomiX::registerFileMenuHook( hookHelper );

} // end of namespace

using namespace KomiX::model::directory;

DirectoryHook::DirectoryHook( QWidget * parent ) : QAction( parent ) {
    setText( tr( "Open &Directory" ) );
    setShortcut( tr( "Ctrl+D" ) );

    connect( this, SIGNAL( triggered() ), this, SLOT( helper_() ) );
    connect( this, SIGNAL( opened( const QUrl & ) ), parent, SLOT( open( const QUrl & ) ) );
}

void DirectoryHook::helper_() {
    QString path = QFileDialog::getExistingDirectory( this->parentWidget(), tr( "Open dicrectory" ), QDir::homePath() );
    if( !path.isEmpty() ) {
        emit opened( QUrl::fromLocalFile( path ) );
    }
}
