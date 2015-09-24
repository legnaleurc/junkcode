#include "conductor.hpp"

#include <QtWidgets/QApplication>


int main (int argc, char * argv[]) {
    QApplication application(argc, argv);

    fbv::Conductor conductor;
    conductor.prepare();

    return application.exec();
}
