#include "server_p.hpp"

#include "session.hpp"

#include <iostream>


using s5p::Server;


Server::Server(boost::asio::io_service & loop, uint16_t port)
    : _(std::make_shared<Server::Private>(loop, port))
{
}

void Server::start() {
    _->doV4Listen();
    _->doV4Accept();
    _->doV6Listen();
    _->doV6Accept();
}

Server::Private::Private(boost::asio::io_service & loop, uint16_t port)
    : port(port)
    , v4_acceptor(loop)
    , v6_acceptor(loop)
    , socket(loop)
{
}

void Server::Private::doV4Listen() {
    EndPoint ep(boost::asio::ip::tcp::v4(), this->port);
    this->v4_acceptor.open(ep.protocol());
    this->v4_acceptor.set_option(Acceptor::reuse_address(true));
    this->v4_acceptor.bind(ep);
    this->v4_acceptor.listen();
}

void Server::Private::doV4Accept() {
    this->v4_acceptor.async_accept(this->socket, [this](const ErrorCode & ec) -> void {
        if (ec) {
            std::cerr << "doV4Accept " << ec.message() << std::endl;
        } else {
            std::make_shared<Session>(std::move(this->socket))->start();
        }

        this->doV4Accept();
    });
}

void Server::Private::doV6Listen() {
    EndPoint ep(boost::asio::ip::tcp::v6(), this->port);
    this->v6_acceptor.open(ep.protocol());
    this->v6_acceptor.set_option(Acceptor::reuse_address(true));
    this->v6_acceptor.set_option(boost::asio::ip::v6_only(true));
    this->v6_acceptor.bind(ep);
    this->v6_acceptor.listen();
}

void Server::Private::doV6Accept() {
    this->v6_acceptor.async_accept(this->socket, [this](const ErrorCode & ec) -> void {
        if (ec) {
            std::cerr << "doV6Accept " << ec.message() << std::endl;
        } else {
            std::make_shared<Session>(std::move(this->socket))->start();
        }

        this->doV6Accept();
    });
}
