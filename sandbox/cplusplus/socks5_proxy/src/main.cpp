#include "server.hpp"
#include "global.hpp"

#include <csignal>
#include <iostream>


int main(int argc, char * argv[]) {
    s5p::IOLoop loop;

    s5p::SignalHandler signals(loop, SIGINT);

    s5p::Server server(loop, 2080);

    signals.async_wait([&loop](const s5p::ErrorCode & ec, int signal_number) -> void {
        if (ec) {
            // TODO handle error
        }
        std::cout << "received " << signal_number << std::endl;
        loop.stop();
    });

    server.start();

    loop.run();

    return 0;
}
