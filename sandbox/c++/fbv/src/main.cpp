#include "keyboardhandler.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>


int main (int argc, char * argv[]) {
    QApplication application(argc, argv);

    fbv::KeyboardHandler kh;

    QGraphicsScene scene;

    QGraphicsView view(&scene);
    auto rect = application.desktop()->availableGeometry();
    view.setGeometry(rect);
    view.show();

    return application.exec();
}
