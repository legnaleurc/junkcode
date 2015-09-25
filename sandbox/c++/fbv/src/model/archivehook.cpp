/**
 * @file archivehook.cpp
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
#include "archivehook.hpp"
#include "archivemodel.hpp"
#include "archive.hpp"
#include "global.hpp"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QFileDialog>

namespace {

inline QString archiveList() {
    QStringList tmp( QObject::tr( "All Supported Archives ( %1 )" ).arg( KomiX::model::archive::ArchiveFormatsFilter().join( " " ) ) );
    tmp << KomiX::model::archive::ArchiveFormatsFilter();
    return tmp.join( ";;" );
}

QAction * hookHelper( QWidget * parent ) {
    return new KomiX::model::archive::ArchiveHook( parent );
}

static const bool registered = KomiX::registerFileMenuHook( hookHelper );

inline const QString & archiveFilter() {
    static QString af = archiveList();
    return af;
}

} // end of namespace

using namespace KomiX::model::archive;

ArchiveHook::ArchiveHook( QWidget * parent ) : QAction( parent ) {
    setText( tr( "Open &Archive" ) );
    setShortcut( tr( "Ctrl+A" ) );

    connect( this, SIGNAL( triggered() ), this, SLOT( helper_() ) );
    connect( this, SIGNAL( opened( const QUrl & ) ), parent, SLOT( open( const QUrl & ) ) );

    // cleanup temporary dir on exit
    this->connect( qApp, SIGNAL( aboutToQuit() ), SLOT( cleanup_() ) );
}

void ArchiveHook::helper_() {
    QString path = QFileDialog::getOpenFileName( this->parentWidget(), tr( "Open archive" ), QDir::homePath(), archiveFilter() );
    if( !path.isEmpty() ) {
        emit opened( QUrl::fromLocalFile( path ) );
    }
}

void ArchiveHook::cleanup_() {
    delTree( getTmpDir() );
}
