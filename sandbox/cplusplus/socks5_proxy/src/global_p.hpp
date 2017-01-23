#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_


#include "global.hpp"

#include <boost/program_options.hpp>


namespace s5p {

typedef boost::program_options::options_description Options;
typedef boost::program_options::variables_map OptionMap;


class Application::Private {
public:
    Private(int argc, char ** argv);

    Options createOptions();
    OptionMap parseOptions(const Options & options) const;
    void onSystemSignal(const ErrorCode & ec, int signal_number);
    void setPort(uint16_t port);
    void setSocks5Host(const std::string & host);
    void setSocks5Port(uint16_t port);
    void setHttpHost(const std::string & host);
    void setHttpPort(uint16_t port);

    IOLoop loop;
    int argc;
    char ** argv;
    uint16_t port;
    std::string socks5_host;
    uint16_t socks5_port;
    std::string http_host;
    uint16_t http_port;
    AddressType http_host_type;
};

}


#endif
