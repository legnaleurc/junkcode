#ifndef FBV_CONDUCTOR_HPP
#define FBV_CONDUCTOR_HPP

#include "keyboardhandler.hpp"
#include "filecontroller.hpp"

#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtCore/QObject>

#include <memory>


namespace fbv {

class Conductor: public QObject {
    Q_OBJECT

public:
    static const QStringList & SupportedFormats();
    static const QStringList & SupportedFormatsFilter();

    Conductor(QObject * parent = 0);

    void prepare(const QString & path);

private slots:
    void _onImageLoaded(QIODevice *);
    void _onUp();
    void _onDown();
    void _onLeft();
    void _onRight();
    void _onQ();

private:
    void _openDirectory(const QString & path);
    void _openFile(const QString & path);

    fbv::KeyboardHandler * _kh;
    fbv::FileController * _fc;
    QStackedWidget * _container;
    QGraphicsScene * _scene;
    QGraphicsView * _view;
};

}

#endif
