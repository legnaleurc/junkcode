#include "global_p.hpp"

#include <iostream>

#if !defined(__APPLE__) && !defined(_WIN32)
#include <endian.h>
#else
#include <boost/endian/conversion.hpp>
#endif


using s5p::Application;
using s5p::IOLoop;
using s5p::Options;
using s5p::OptionMap;
using s5p::ErrorCode;


static Application * singleton = nullptr;


Application & Application::instance() {
    return *singleton;
}


Application::Application(int argc, char ** argv)
    : _(std::make_shared<Private>(argc, argv))
{
    singleton = this;
}

int Application::prepare() {
    auto options = _->createOptions();
    OptionMap args;
    try {
        args = _->parseOptions(options);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (args.empty() || args.count("help") >= 1) {
        std::cout << options << std::endl;
        return -1;
    }

    return 0;
}

IOLoop & Application::ioloop() const {
    return _->loop;
}

int Application::exec() {
    namespace ph = std::placeholders;

    s5p::SignalHandler signals(_->loop, SIGINT, SIGTERM);
    signals.async_wait(std::bind(&Application::Private::onSystemSignal, _, ph::_1, ph::_2));

    _->loop.run();
    return 0;
}

Application::Private::Private(int argc, char ** argv)
    : loop()
    , argc(argc)
    , argv(argv)
{
}

Options Application::Private::createOptions() const {
    namespace po = boost::program_options;

    Options od("SOCKS5 proxy");
    od.add_options()
        ("help,h", "show this message")
        ("listen,l", po::value<uint16_t>(), "listen to the port")
        ("socks5-host", po::value<std::string>(), "SOCKS5 host")
        ("socks5-port", po::value<uint16_t>(), "SOCKS5 port")
        ("http-host", po::value<std::string>(), "forward to this host")
        ("http-port", po::value<uint16_t>(), "forward to this port")
    ;
    return std::move(od);
}

OptionMap Application::Private::parseOptions(const Options & options) const {
    namespace po = boost::program_options;

    OptionMap vm;
    auto rv = po::parse_command_line(argc, argv, options);
    po::store(rv, vm);
    po::notify(vm);

    return vm;
}

void Application::Private::onSystemSignal(const ErrorCode & ec, int signal_number) {
    if (ec) {
        // TODO handle error
        std::cerr << "signal " << ec.message() << std::endl;
    }
    std::cout << "received " << signal_number << std::endl;
    this->loop.stop();
}


namespace s5p {

Chunk createChunk() {
    return std::move(Chunk());
}

void putBigEndian(uint8_t * dst, uint16_t native) {
    uint16_t * view = reinterpret_cast<uint16_t *>(dst);
#if !defined(__APPLE__) && !defined(_WIN32)
    *view = htobe16(native);
#else
    *view = boost::endian::native_to_big(native);
#endif
}

}
