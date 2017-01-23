#ifndef SERVER_HPP
#define SERVER_HPP

#include "global.hpp"

#include <memory>


namespace s5p {

class Server {
public:
    explicit Server(IOLoop & loop);

    void listenV4(uint16_t port);
    void listenV6(uint16_t port);

private:
    Server(const Server &);
    Server & operator = (const Server &);
    Server(Server &&);
    Server & operator = (Server &&);

    class Private;
    std::shared_ptr<Private> _;
};

}

#endif
