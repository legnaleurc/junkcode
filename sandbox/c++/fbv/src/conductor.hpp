#ifndef FBV_CONDUCTOR_HPP
#define FBV_CONDUCTOR_HPP

#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtCore/QObject>

#include <memory>


namespace fbv {

class KeyboardHandler;
class FileController;

namespace widget {

class ImageItem;

}

class Conductor: public QObject {
    Q_OBJECT

public:
    static const QStringList & SupportedFormats();
    static const QStringList & SupportedFormatsFilter();

    Conductor(QObject * parent = 0);

    void prepare(const QString & path);

private slots:
    void _onImageLoaded(QIODevice *);
    void _onImageChanged();
    void _onUp();
    void _onDown();
    void _onLeft();
    void _onRight();
    void _onPageUp();
    void _onPageDown();
    void _onQ();

private:
    QRectF _getCurrentImageRect() const;
    QRectF _getViewportRect() const;
    void _updateScaling();
    void _fitWindow();
    void _fitWidth();
    void _fitHeight();
    void _scale(double ratio);
    void _fromViewportMoveBy();
    QLineF _normalizeMotionVector(const QPointF & delta);
    void _moveBy(const QPointF & delta);

    fbv::KeyboardHandler * _kh;
    fbv::FileController * _fc;
    QStackedWidget * _container;
    QGraphicsScene * _scene;
    QGraphicsView * _view;
    fbv::widget::ImageItem * _currentImage;
    double _currentRatio;
};

}

#endif
