/**
 * @file archivemodel.cpp
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
#include "archivemodel_p.hpp"
#include "archive.hpp"
#include "exception.hpp"
#include "global.hpp"

#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QtDebug>
#include <QtGui/QPixmap>
#include <QtWidgets/QApplication>

namespace KomiX {
namespace exception {

/// Private archive error class
class ArchiveException: public Exception {
public:
    ArchiveException( const char * msg ): Exception( msg ) {
    }
    ArchiveException( const QString & msg ): Exception( msg ) {
    }
};

}
} // end of namespace

namespace {

bool check( const QUrl & url ) {
    if( url.scheme() == "file" ) {
        QFileInfo fi( url.toLocalFile() );
        if( !fi.isDir() ) {
            return KomiX::model::archive::isArchiveSupported( fi.fileName().toLower() );
        }
    }
    return false;
}

std::shared_ptr< KomiX::model::FileModel > create( const QUrl & url ) {
    if( !KomiX::model::archive::ArchiveModel::IsRunnable() ) {
        throw KomiX::exception::ArchiveException( "This feature is based on 7-zip. Please install it." );
    } else if( !KomiX::model::archive::ArchiveModel::IsPrepared() ) {
        throw KomiX::exception::ArchiveException( "I could not create temporary directory." );
    }
    return std::shared_ptr< KomiX::model::FileModel >( new KomiX::model::archive::ArchiveModel( QFileInfo( url.toLocalFile() ) ) );
}

static const bool registered = KomiX::model::FileModel::registerModel( check, create );

inline const QStringList & archiveList2() {
    static QStringList a2 = QStringList() << "tar.gz" << "tgz" << "tar.bz2" << "tbz2" << "tar.lzma";
    return a2;
}

inline bool isTwo( const QString & name ) {
    foreach( QString ext, archiveList2() ) {
        if( name.endsWith( ext ) ) {
            return true;
        }
    }
    return false;
}

inline QStringList archiveList() {
    QStringList a( archiveList2() );
    a << "7z";
    a << "rar";
    a << "zip";
    a << "tar";
    return a;
}

const QString & sevenZip() {
#ifdef Q_OS_WIN32
    static QString sz = "C:\\Program Files\\7-Zip\\7z.exe";
#elif defined( Q_OS_UNIX )
    static QString sz = "/usr/local/bin/7z";
#endif
    return sz;
}

QDir archiveDir( const QString & dirName ) {
    if( !KomiX::model::archive::getTmpDir().exists( dirName ) ) {
        KomiX::model::archive::getTmpDir().mkdir( dirName );
    }
    QDir tmp( KomiX::model::archive::getTmpDir() );
    tmp.cd( dirName );
    return tmp;
}

QStringList arguments( const QString & fileName ) {
    QStringList args( "e" );
    args << QString( "-o%1" ).arg( archiveDir( fileName ).absolutePath() );
    args << "-aos";
    return args;
}

} // end of namespace

using KomiX::model::archive::ArchiveModel;

ArchiveModel::Private::Private( ArchiveModel * owner, const QFileInfo & root ):
QObject(),
owner( owner ),
root( root ),
hash() {
}

void ArchiveModel::Private::extract( const QString & aFilePath, const char * onFinished ) {
    QProcess * p = new QProcess;
    this->connect( p, SIGNAL( finished( int ) ), onFinished );
    this->connect( p, SIGNAL( finished( int ) ), SLOT( cleanup( int ) ) );
    p->start( sevenZip(), ( arguments( this->hash ) << aFilePath ),  QIODevice::ReadOnly );
}

void ArchiveModel::Private::cleanup( int exitCode ) {
    QProcess * p = static_cast< QProcess * >( this->sender() );
    if( exitCode != 0 ) {
        // delete wrong dir
        KomiX::model::archive::delTree( archiveDir( hash ) );
        QString err = QString::fromLocal8Bit( p->readAllStandardError() );
        qWarning() << p->readAllStandardOutput();
        qWarning() << err;
        emit this->error( err );
    }
    p->deleteLater();
}

void ArchiveModel::Private::checkTwo( int exitCode ) {
    if( exitCode != 0 ) {
        return;
    }
    // check if is tar-compressed
    if( isTwo( this->root.fileName() ) ) {
        QString name = archiveDir( this->hash ).absoluteFilePath( this->root.completeBaseName() );
        this->extract( name, SLOT( allDone( int ) ) );
    } else {
        this->owner->setRoot( archiveDir( this->hash ) );
        emit this->ready();
    }
}

void ArchiveModel::Private::allDone( int exitCode ) {
    if( exitCode != 0 ) {
        return;
    }
    this->owner->setRoot( archiveDir( this->hash ) );
    emit this->ready();
}

bool ArchiveModel::IsRunnable() {
    return QFileInfo( sevenZip() ).isExecutable();
}

bool ArchiveModel::IsPrepared() {
    return QDir::temp() != getTmpDir();
}

ArchiveModel::ArchiveModel( const QFileInfo & root ):
LocalFileModel(),
p_( new Private( this, root ) ) {
    this->connect( this->p_.get(), SIGNAL( error( const QString & ) ), SIGNAL( error( const QString & ) ) );
    this->connect( this->p_.get(), SIGNAL( ready() ), SIGNAL( ready() ) );
}

void ArchiveModel::doInitialize() {
    this->p_->hash = QString::fromUtf8( QCryptographicHash::hash( this->p_->root.fileName().toUtf8(), QCryptographicHash::Sha1 ).toHex() );

    if( getTmpDir().exists( this->p_->hash ) ) {
        // uncompressed before
        this->setRoot( archiveDir( this->p_->hash ) );
        emit this->ready();
        return;
    }

    this->p_->extract( this->p_->root.absoluteFilePath(), SLOT( checkTwo( int ) ) );
}

namespace KomiX {
    namespace model {
        namespace archive {

            const QStringList & ArchiveFormats() {
                static QStringList af = archiveList();
                return af;
            }

            const QStringList & ArchiveFormatsFilter() {
                static QStringList sff = toNameFilter( ArchiveFormats() );
                return sff;
            }

            bool isArchiveSupported( const QString & name ) {
                foreach( QString ext, ArchiveFormats() ) {
                    if( name.endsWith( ext ) ) {
                        return true;
                    }
                }
                return false;
            }

        }
    }
}
