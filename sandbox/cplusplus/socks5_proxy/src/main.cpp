#include "server.hpp"
#include "global.hpp"

#include <csignal>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;


int main(int argc, char * argv[]) {
    po::options_description od("SOCKS5 proxy");
    od.add_options()
        ("help,h", "show this message")
        ("listen,l", po::value<uint16_t>(), "listen to the port")
        ("host,H", po::value<std::string>(), "forward to this host")
        ("port,p", po::value<uint16_t>(), "forward to this port")
    ;

    po::variables_map vm;
    try {
        auto rv = po::parse_command_line(argc, argv, od);
        po::store(rv, vm);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << od << std::endl;
        return 0;
    }

    return 0;

    s5p::IOLoop loop;

    s5p::SignalHandler signals(loop, SIGINT);

    s5p::Server server(loop);

    signals.async_wait([&loop](const s5p::ErrorCode & ec, int signal_number) -> void {
        if (ec) {
            // TODO handle error
        }
        std::cout << "received " << signal_number << std::endl;
        loop.stop();
    });

    server.listenV4(2080);
    server.listenV6(2080);

    loop.run();

    return 0;
}
