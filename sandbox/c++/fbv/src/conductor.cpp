#include "conductor.hpp"
#include "widget/imageitem.hpp"
#include "keyboardhandler.hpp"
#include "filecontroller.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QImageReader>
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
using fbv::widget::ImageItem;


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

    this->connect(this->_fc, SIGNAL(imageLoaded(QIODevice*)), SLOT(_onImageLoaded(QIODevice*)));

    this->connect(this->_kh, SIGNAL(up()), SLOT(_onUp()));
    this->connect(this->_kh, SIGNAL(down()), SLOT(_onDown()));
    this->connect(this->_kh, SIGNAL(left()), SLOT(_onLeft()));
    this->connect(this->_kh, SIGNAL(right()), SLOT(_onRight()));
    this->connect(this->_kh, SIGNAL(pageUp()), SLOT(_onPageUp()));
    this->connect(this->_kh, SIGNAL(pageDown()), SLOT(_onPageDown()));
    this->connect(this->_kh, SIGNAL(q()), SLOT(_onQ()));

    this->_container->addWidget(this->_view);
    this->_container->setCurrentIndex(0);

    auto rect = qApp->desktop()->availableGeometry();
    this->_container->setGeometry(rect);
    this->_container->show();

    auto url = QUrl::fromLocalFile(path);
    this->_fc->open(url);
}

void Conductor::_onImageLoaded(QIODevice * image) {
    this->_scene->clear();

    QList<QIODevice *> images({image});
    auto images_ = new ImageItem(images);
    this->connect(images_, SIGNAL(changed()), SLOT(_onImageChanged()));
    this->_scene->addItem(images_);

    this->_onImageChanged();
}

void Conductor::_onImageChanged() {
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

void Conductor::_onPageUp() {
    this->_fc->prev();
}

void Conductor::_onPageDown() {
    this->_fc->next();
}

void Conductor::_onQ() {
    this->_container->close();
}
