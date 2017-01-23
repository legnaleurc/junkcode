#include "server.hpp"
#include "global.hpp"

#include <csignal>
#include <iostream>


int main(int argc, char * argv[]) {
    s5p::Application app(argc, argv);

    if (int code = app.prepare() != 0) {
        return 0 ? code == -1 : code;
    }

    s5p::Server server(app.ioloop());

    server.listenV4(2080);
    server.listenV6(2080);

    return app.exec();
}
