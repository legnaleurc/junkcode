#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <boost/asio.hpp>


namespace s5p {

typedef std::array<uint8_t, 8192> Chunk;
typedef boost::asio::io_service IOLoop;
typedef boost::asio::ip::tcp::acceptor Acceptor;
typedef boost::asio::ip::tcp::endpoint EndPoint;
typedef boost::asio::ip::tcp::resolver Resolver;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::system::error_code ErrorCode;


enum AddressType {
    IPV4,
    IPV6,
    FQDN,
    UNKNOWN,
};


class Application {
public:
    static Application & instance();

    Application(int argc, char ** argv);

    int prepare();

    IOLoop & ioloop() const;
    uint16_t port() const;
    const std::string & socks5Host() const;
    uint16_t socks5Port() const;
    const std::string & httpHost() const;
    uint16_t httpPort() const;
    AddressType httpHostType() const;

    int exec();

private:
    Application(const Application &);
    Application & operator = (const Application &);
    Application(Application &&);
    Application & operator = (Application &&);

    class Private;
    std::shared_ptr<Private> _;
};


Chunk createChunk();
void putBigEndian(uint8_t * dst, uint16_t native);

}

#endif
