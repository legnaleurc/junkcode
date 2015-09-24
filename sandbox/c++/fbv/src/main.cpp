#include "conductor.hpp"

#include <QtWidgets/QApplication>


int main (int argc, char * argv[]) {
    QApplication application(argc, argv);

    auto args = application.arguments();
    if (args.size() != 2) {
        return 1;
    }

    fbv::Conductor conductor;
    conductor.prepare(args.at(1));

    return application.exec();
}
