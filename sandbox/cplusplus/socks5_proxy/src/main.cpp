#include "server.hpp"
#include "global.hpp"

#include <boost/asio.hpp>

#include <csignal>


int main(int argc, char * argv[]) {
    boost::asio::io_service loop;

    boost::asio::signal_set signals(loop, SIGINT);

    s5p::Server server(loop, 8000);

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
