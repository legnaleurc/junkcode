/**
 * @file singlehook.cpp
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
#include "global.hpp"
#include "singlehook.hpp"

#include <QtCore/QUrl>
#include <QtWidgets/QFileDialog>

namespace {

QAction * hookHelper( QWidget * parent ) {
    return new KomiX::model::single::SingleHook( parent );
}

const bool registered = KomiX::registerFileMenuHook( hookHelper );

inline QString formatList() {
    QMultiMap< QString, QString > cat;

    foreach( QString str, KomiX::SupportedFormats() ) {
        if( str == "bmp" ) {
            cat.insert( QObject::tr( "Windows Bitmap" ), str.prepend( "*." ) );
        } else if( str == "gif" ) {
            cat.insert( QObject::tr( "Graphic Interchange Format" ), str.prepend( "*." ) );
        } else if( str == "jpg" || str == "jpeg" || str == "jp2" ) {
            cat.insert( QObject::tr( "Joint Photographic Experts Group" ), str.prepend( "*." ) );
        } else if( str == "mng" || str == "png" ) {
            cat.insert( QObject::tr( "Network Graphics" ), str.prepend( "*." ) );
        } else if( str == "pbm" || str == "pgm" || str == "ppm" ) {
            cat.insert( QObject::tr( "Portable anymap" ), str.prepend( "*." ) );
        } else if( str == "tif" || str == "tiff" ) {
            cat.insert( QObject::tr( "Tagged Image File Format" ), str.prepend( "*." ) );
        } else if( str == "xbm" || str == "xpm" ) {
            cat.insert( QObject::tr( "X11" ), str.prepend( "*." ) );
        } else if( str == "sgi" || str == "rgba" || str == "rgb" || str == "bw" ) {
            cat.insert( QObject::tr( "Silicon Graphics Image" ), str.prepend( "*." ) );
        } else if( str == "tga" ) {
            cat.insert( QObject::tr( "Truevision Advanced Raster Graphics Adapter" ), str.prepend( "*." ) );
        } else if( str == "eps" || str == "epsf" || str == "epsi" ) {
            cat.insert( QObject::tr( "Encapsulated PostScript" ), str.prepend( "*." ) );
        } else if( str == "svg" ) {
            cat.insert( QObject::tr( "Scalable Vector Graphics" ), str.prepend( "*." ) );
        } else if( str == "xcf" ) {
            cat.insert( QObject::tr( "eXperimental Computing Facility" ), str.prepend( "*." ) );
        } else if( str == "psd" ) {
            cat.insert( QObject::tr( "Photoshop Document" ), str.prepend( "*." ) );
        } else if( str == "pcx" ) {
            cat.insert( QObject::tr( "PC Paintbrush Exchange" ), str.prepend( "*." ) );
        } else if( str == "ico" ) {
            cat.insert( QObject::tr( "Windows icon" ), str.prepend( "*." ) );
        } else if( str == "dds" ) {
            cat.insert( QObject::tr( "DirectDraw Surface" ), str.prepend( "*." ) );
        } else if( str == "exr" ) {
            cat.insert( QObject::tr( "OpenEXR" ), str.prepend( "*." ) );
        } else {
            cat.insert( QObject::tr( "Others" ), str.prepend( "*." ) );
        }
    }

    QStringList tmp;
    foreach( QString key, cat.uniqueKeys() ) {
        tmp.push_back( key + " ( " + QStringList( cat.values( key ) ).join( " " ) + " )" );
    }
    tmp.push_front( QObject::tr( "All Supported File" ) + " ( " + KomiX::SupportedFormatsFilter().join( " " ) + " )" );

    return tmp.join( ";;" );
}

inline const QString & fileFilter() {
    static QString ff = formatList();
    return ff;
}

} // end of namespace

using namespace KomiX::model::single;

SingleHook::SingleHook( QWidget * parent ) : QAction( parent ) {
    setText( tr( "&Open Image" ) );
    setShortcut( tr( "Ctrl+O" ) );

    connect( this, SIGNAL( triggered() ), this, SLOT( helper_() ) );
    connect( this, SIGNAL( opened( const QUrl & ) ), parent, SLOT( open( const QUrl & ) ) );
}

void SingleHook::helper_() {
    QString path = QFileDialog::getOpenFileName( this->parentWidget(), tr( "Open image file" ), QDir::homePath(), fileFilter() );
    if( !path.isEmpty() ) {
        emit opened( QUrl::fromLocalFile( path ) );
    }
}
