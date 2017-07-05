#include <QtWidgets/QApplication>

#include "widget/mainwindow.hpp"


int main (int argc, char * argv[]) {
    QApplication app(argc, argv);

    qtfs::MainWindow mw;
    mw.show();

    return app.exec();
}
