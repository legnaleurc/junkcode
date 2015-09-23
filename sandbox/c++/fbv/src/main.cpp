#include "keyboardhandler.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QMovie>


int main (int argc, char * argv[]) {
    QApplication application(argc, argv);
    auto rect = application.desktop()->availableGeometry();

    fbv::KeyboardHandler kh;

    // root
    QStackedWidget container;
    QGraphicsScene * scene = new QGraphicsScene(&kh);
    QGraphicsView * view = new QGraphicsView(scene, &container);

    QLabel * label = new QLabel;
    QMovie * movie = new QMovie("/tmp/example.gif");
    label->setMovie(movie);
    movie->start();

    auto wpItem = scene->addWidget(label);

    container.addWidget(view);
    container.setCurrentIndex(0);

    container.setGeometry(rect);
    container.show();

    return application.exec();
}
