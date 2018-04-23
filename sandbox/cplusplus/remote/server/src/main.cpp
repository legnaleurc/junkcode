#include <QtCore/QCoreApplication>

#include "server.hpp"


int main(int argc, char * argv[]) {
    QCoreApplication app{argc, argv};

    remote::Server server;
    server.start();

    return app.exec();
}
