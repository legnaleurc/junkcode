#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "server.hpp"

#include "global.hpp"


namespace s5p {

class Server::Private {
public:
    Private(boost::asio::io_service & loop, uint16_t port);

    void doV4Listen();
    void doV4Accept();
    void doV6Listen();
    void doV6Accept();

    uint16_t port;
    Acceptor v4_acceptor;
    Acceptor v6_acceptor;
    Socket socket;
};

}

#endif
