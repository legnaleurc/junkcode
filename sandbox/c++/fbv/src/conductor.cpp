#include "conductor.hpp"

#include <QtWidgets/QApplication>
#include <QtCore/QFileInfo>
#include <QtWidgets/QLabel>
#include <QtGui/QMovie>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QMimeDatabase>
#include <QtCore/QtDebug>


namespace {

QStringList toNameFilter( const QStringList & exts ) {
    QStringList tmp;
    foreach( QString str, exts ) {
        tmp << str.prepend( "*." );
    }
    return tmp;
}

QStringList uniqueList() {
    Q_ASSERT( QCoreApplication::instance() != NULL );
    std::list< QByteArray > uniList = QImageReader::supportedImageFormats().toStdList();

    std::for_each( uniList.begin(), uniList.end(), []( QByteArray & s )->void {
        s = s.toLower();
    } );
    uniList.sort();
    uniList.unique();

    QStringList result;

    std::copy( uniList.begin(), uniList.end(), std::back_inserter( result ) );
    return result;
}

}


using fbv::Conductor;
using fbv::KeyboardHandler;
using fbv::FileController;


const QStringList & Conductor::SupportedFormats() {
    static QStringList sf = uniqueList();
    return sf;
}

const QStringList & Conductor::SupportedFormatsFilter() {
    static QStringList sff = toNameFilter( Conductor::SupportedFormats() );
    return sff;
}

Conductor::Conductor(QObject * parent):
QObject(parent),
_kh(new KeyboardHandler(this)),
_fc(new FileController(this)),
_container(new QStackedWidget) {
}

void Conductor::prepare(const QString & path) {
    QFileInfo fi(path);
    if (fi.isDir()) {
        // open dir model
    } else {
        QMimeDatabase db;
        auto mime = db.mimeTypeForFile(fi);
    }

    this->connect(this->_kh, SIGNAL(up()), SLOT(_onUp()));
    this->connect(this->_kh, SIGNAL(down()), SLOT(_onDown()));
    this->connect(this->_kh, SIGNAL(left()), SLOT(_onLeft()));
    this->connect(this->_kh, SIGNAL(right()), SLOT(_onRight()));
    this->connect(this->_kh, SIGNAL(q()), SLOT(_onQ()));

    auto rect = qApp->desktop()->availableGeometry();

    QGraphicsScene * scene = new QGraphicsScene(this->_kh);
    QGraphicsView * view = new QGraphicsView(scene, this->_container);

    QLabel * label = new QLabel;
    QMovie * movie = new QMovie("/tmp/1.gif");
    label->setMovie(movie);
    movie->start();

    auto wpItem = scene->addWidget(label);

    this->_container->addWidget(view);
    this->_container->setCurrentIndex(0);

    this->_container->setGeometry(rect);
    this->_container->show();
}

void Conductor::_openDirectory(const QString & path) {
    ;
}

void Conductor::_openFile(const QString & path) {
    ;
}

void Conductor::_onUp() {
    qDebug() << "up";
}

void Conductor::_onDown() {
    qDebug() << "down";
}

void Conductor::_onLeft() {
    qDebug() << "left";
}

void Conductor::_onRight() {
    qDebug() << "right";
}

void Conductor::_onQ() {
    this->_container->close();
}
