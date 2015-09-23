#include "keyboardhandler.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QFileDialog>


int main (int argc, char * argv[]) {
    QApplication application(argc, argv);
    auto rect = application.desktop()->availableGeometry();

    fbv::KeyboardHandler kh;

    // root
    QStackedWidget container;
    QGraphicsScene * scene = new QGraphicsScene(&kh);
    QGraphicsView * view = new QGraphicsView(scene, &container);
//    QFileDialog * fileDialog = new QFileDialog(&container, Qt::Widget);

    container.addWidget(view);
//    container.addWidget(fileDialog);
    container.setCurrentIndex(0);

    container.setGeometry(rect);
    container.show();

    return application.exec();
}
