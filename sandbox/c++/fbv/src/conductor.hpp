#ifndef FBV_CONDUCTOR_HPP
#define FBV_CONDUCTOR_HPP

#include "keyboardhandler.hpp"

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QMovie>
#include <QtCore/QtDebug>
#include <QtCore/QObject>

#include <memory>


namespace fbv {

class Conductor: public QObject {
    Q_OBJECT

public:
    Conductor(QObject * parent = 0);

    void prepare();

private slots:
    void _onUp();
    void _onDown();
    void _onLeft();
    void _onRight();
    void _onQ();

private:
    fbv::KeyboardHandler * _kh;
    QStackedWidget * _container;
};

}

#endif
