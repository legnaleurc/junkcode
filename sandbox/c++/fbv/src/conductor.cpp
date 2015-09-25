#include "conductor.hpp"

#include <QtWidgets/QApplication>
#include <QtCore/QFileInfo>
#include <QtWidgets/QLabel>
#include <QtGui/QMovie>
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
_kh(),
_fc(),
_container(),
_scene(),
_view() {
}

void Conductor::prepare(const QString & path) {
    this->_kh = new KeyboardHandler(this);
    this->_fc = new FileController(this);
    this->_container = new QStackedWidget;
    this->_scene = new QGraphicsScene(this);
    this->_view = new QGraphicsView(this->_scene, this->_container);

    auto url = QUrl::fromLocalFile(path);
    this->_fc->open(url);
//    QFileInfo fi(path);
//    if (fi.isDir()) {
//        // open dir model
//    } else {
//        QMimeDatabase db;
//        auto mime = db.mimeTypeForFile(fi);
//    }

    this->connect(this->_fc, SIGNAL(imageLoaded(QIODevice*)), SLOT(_onImageLoaded(QIODevice*)));

    this->connect(this->_kh, SIGNAL(up()), SLOT(_onUp()));
    this->connect(this->_kh, SIGNAL(down()), SLOT(_onDown()));
    this->connect(this->_kh, SIGNAL(left()), SLOT(_onLeft()));
    this->connect(this->_kh, SIGNAL(right()), SLOT(_onRight()));
    this->connect(this->_kh, SIGNAL(q()), SLOT(_onQ()));

    auto rect = qApp->desktop()->availableGeometry();

//    QLabel * label = new QLabel;
//    QMovie * movie = new QMovie("/tmp/1.gif");
//    label->setMovie(movie);
//    movie->start();

//    auto wpItem = scene->addWidget(label);

    this->_container->addWidget(this->_view);
    this->_container->setCurrentIndex(0);

    this->_container->setGeometry(rect);
    this->_container->show();
}

void Conductor::_onImageLoaded(QIODevice * image) {
    this->_scene->clear();
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
