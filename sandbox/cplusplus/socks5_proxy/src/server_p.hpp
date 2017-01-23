#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "server.hpp"


namespace s5p {

class Server::Private {
public:
    explicit Private(IOLoop & loop);

    void doV4Listen(uint16_t port);
    void doV4Accept();
    void doV6Listen(uint16_t port);
    void doV6Accept();

    Acceptor v4_acceptor;
    Acceptor v6_acceptor;
    Socket socket;
};

}

#endif
