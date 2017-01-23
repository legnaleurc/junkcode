#include "server.hpp"
#include "global.hpp"

#include <csignal>


int main(int argc, char * argv[]) {
    s5p::IOLoop loop;

    s5p::SignalHandler signals(loop, SIGINT);

    s5p::Server server(loop, 1080);

    signals.async_wait([&loop](const s5p::ErrorCode & ec, int signal_number) -> void {
        if (ec) {
            // TODO handle error
        }
        loop.stop();
    });

    server.start();

    loop.run();

    return 0;
}
