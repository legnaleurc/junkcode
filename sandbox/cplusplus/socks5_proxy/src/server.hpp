#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

#include <memory>


namespace s5p {

class Server {
public:
    Server(boost::asio::io_service & loop, uint16_t port);

    void start();

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
