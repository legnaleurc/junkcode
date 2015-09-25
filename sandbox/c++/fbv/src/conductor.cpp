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
_view(),
_currentImage(),
_currentRatio(1.0) {
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
    if (!this->_currentImage || this->_currentImage->boundingRect().isNull()) {
        return;
    }

    this->_scene->setSceneRect(this->_currentImage->boundingRect());
    this->_currentImage->setPos(0, 0);

    this->_updateScaling();
}

QRectF Conductor::_getCurrentImageRect() const {
    if (!this->_currentImage) {
        return QRectF();
    }
    return this->_currentImage->sceneBoundingRect();
}

QRectF Conductor::_getViewportRect() const {
    auto vpr = this->_view->viewport()->rect();
    return this->_view->mapToScene(vpr).boundingRect();
}

void Conductor::_updateScaling() {
    // TODO add scale mode
    this->_fitWindow();
}

void Conductor::_fitWindow() {
    auto cir = this->_getCurrentImageRect();
    auto vpr = this->_getViewportRect();
    double dW = cir.width() - vpr.width();
    double dH = cir.height() - vpr.height();

    if (dW <= 0.0 && dH <= 0.0) {
        return;
    }

    if(dW > dH) {
        this->_fitWidth();
    } else {
        this->_fitHeight();
    }
}

void Conductor::_fitWidth() {
    auto cir = this->_getCurrentImageRect();
    auto vpr = this->_getViewportRect();
    this->_scale(vpr.width() / cir.width());
}

void Conductor::_fitHeight() {
    auto cir = this->_getCurrentImageRect();
    auto vpr = this->_getViewportRect();
    this->_scale(vpr.height() / cir.height());
}

void Conductor::_scale(double ratio) {
    this->_view->scale(ratio, ratio);
    this->_fromViewportMoveBy();
    this->_currentRatio *= ratio;
}

void Conductor::_fromViewportMoveBy() {
    QPointF delta;
    delta /= this->_currentRatio;
    QLineF v = this->_normalizeMotionVector(delta);
    this->_moveBy(v.p2());
}

QLineF Conductor::_normalizeMotionVector(const QPointF & delta) {
    double dx = delta.x();
    double dy = delta.y();
    auto cir = this->_getCurrentImageRect();
    auto vpr = this->_getViewportRect();
    QRectF req = cir.translated( dx, dy );
    // horizontal
    if (cir.width() < vpr.width()) {
        dx += vpr.center().x() - req.center().x();
    } else if (req.left() > vpr.left()) {
        dx += vpr.left() - req.left();
    } else if (req.right() < vpr.right()) {
        dx += vpr.right() - req.right();
    }
    // vertical
    if (cir.height() < vpr.height()) {
        dy += vpr.center().y() - req.center().y();
    } else if (req.top() > vpr.top()) {
        dy += vpr.top() - req.top();
    } else if (req.bottom() < vpr.bottom()) {
        dy += vpr.bottom() - req.bottom();
    }
    return QLineF(0, 0, dx, dy);
}

void Conductor::_moveBy(const QPointF & delta) {
    if (!this->_currentImage) {
        return;
    }
    this->_currentImage->moveBy(delta.x(), delta.y());

    auto cir = this->_getCurrentImageRect();
    auto vpr = this->_getViewportRect();
    auto img = cir.center();
    auto vp = vpr.center();
}

void Conductor::_onUp() {
    qDebug() << "up";
    qDebug() << QApplication::focusWidget();
}

void Conductor::_onDown() {
    qDebug() << "down";
    qDebug() << QApplication::focusWidget();
}

void Conductor::_onLeft() {
    qDebug() << "left";
    qDebug() << QApplication::focusWidget();
}

void Conductor::_onRight() {
    qDebug() << "right";
    qDebug() << QApplication::focusWidget();
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
