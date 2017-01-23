#include "session_p.hpp"

#include "global.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>


using s5p::Session;
using s5p::Socket;
using s5p::Resolver;
using s5p::ErrorCode;
using s5p::Chunk;


Session::Session(Socket socket)
    : _(std::make_shared<Session::Private>(std::move(socket)))
{
}

void Session::start() {
    _->self = this->shared_from_this();
    _->doInnerResolve();
}

Session::Private::Private(Socket socket)
    : self()
    , outer_socket(std::move(socket))
    , inner_socket(this->outer_socket.get_io_service())
    , incoming_buffer(createChunk())
    , outgoing_buffer(createChunk())
    , resolver(this->outer_socket.get_io_service())
{
}

std::shared_ptr<Session> Session::Private::kungFuDeathGrip() {
    return this->self.lock();
}

void Session::Private::doInnerResolve() {
    auto self = this->kungFuDeathGrip();
    auto fn = [self](const ErrorCode & ec, Resolver::iterator it) -> void {
        self->_->onResolved(ec, it);
    };

    auto port = boost::lexical_cast<std::string>(Application::instance().socks5Port());
    this->resolver.async_resolve({
        Application::instance().socks5Host(),
        port,
    }, fn);
}

void Session::Private::onResolved(const ErrorCode & ec, Resolver::iterator it) {
    // TODO handle error
    if (ec) {
        std::cerr << "onResolved failed" << ec << std::endl;
        return;
    }

    this->doInnerConnect(it);
}

void Session::Private::doInnerConnect(Resolver::iterator it) {
    if (Resolver::iterator() == it) {
        return;
    }

    auto self = this->kungFuDeathGrip();
    auto fn = [self, it](const ErrorCode & ec) -> void {
        self->_->onInnerConnected(ec, it);
    };

    this->inner_socket.async_connect(*it, fn);
}

void Session::Private::onInnerConnected(const ErrorCode & ec, Resolver::iterator it) {
    if (ec) {
        std::cerr << "onInnerConnected failed " << ec.message() << std::endl;
        this->inner_socket.close();
        it = std::next(it);
        this->doInnerConnect(it);
        return;
    }

    this->doInnerPhase1();
}

void Session::Private::doInnerPhase1() {
    auto & buffer = this->incoming_buffer;
    // VER
    buffer[0] = 0x05;
    // NMETHODS
    buffer[1] = 0x01;
    // METHODS
    buffer[2] = 0x00;

    this->doInnerPhase1Write(0, 3);
}

void Session::Private::doInnerPhase1Write(std::size_t offset, std::size_t length) {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(&this->incoming_buffer[offset], length);
    auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
        self->_->onInnerPhase1Wrote(ec, offset, length, wrote_length);
    };
    this->inner_socket.async_write_some(buffer, fn);
}

void Session::Private::onInnerPhase1Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
    // TODO handle error
    if (ec) {
        std::cerr << "onInnerPhase1Wrote " << ec.message() << std::endl;
        return;
    }

    if (wrote_length < total_length) {
        this->doInnerPhase1Write(offset + wrote_length, total_length - wrote_length);
    } else {
        this->doInnerPhase2();
    }
}

void Session::Private::doInnerPhase2() {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(this->outgoing_buffer);
    auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
        self->_->onInnerPhase2Read(ec, length);
    };
    this->inner_socket.async_read_some(buffer, fn);
}

void Session::Private::onInnerPhase2Read(const ErrorCode & ec, std::size_t length) {
    if (ec) {
        std::cerr << "onInnerPhase2Read " << ec.message() << std::endl;
        return;
    }

    if (length < 2) {
        std::cerr << "onInnerPhase2Read wrong auth header length" << std::endl;
        return;
    }

    if (this->outgoing_buffer[1] != 0x00) {
        std::cerr << "onInnerPhase2Read provided auth not supported" << std::endl;
        return;
    }

    this->doInnerPhase3();
}

void Session::Private::doInnerPhase3() {
    auto & buffer = this->incoming_buffer;
    // VER
    buffer[0] = 0x05;
    // CMD
    buffer[1] = 0x01;
    // RSV
    buffer[2] = 0x00;

    std::size_t used_byte = 0;
    switch (Application::instance().httpHostType()) {
    case AddressType::IPV4:
        used_byte = this->fillIpv4(buffer, 3);
        break;
    case AddressType::IPV6:
        used_byte = this->fillIpv6(buffer, 3);
        break;
    case AddressType::FQDN:
        used_byte = this->fillFqdn(buffer, 3);
        break;
    default:
        std::cerr << "unknown target http address" << std::endl;
        return;
    }

    // DST.PORT
    putBigEndian(&buffer[3 + used_byte], Application::instance().httpPort());

    std::size_t total_length = 3 + used_byte + 2;
    this->doInnerPhase3Write(0, total_length);
}

void Session::Private::doInnerPhase3Write(std::size_t offset, std::size_t length) {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(&this->incoming_buffer[offset], length);
    auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
        self->_->onInnerPhase3Wrote(ec, offset, length, wrote_length);
    };
    this->inner_socket.async_write_some(buffer, fn);
}

void Session::Private::onInnerPhase3Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
    // TODO handle error
    if (ec) {
        std::cerr << "onInnerPhase3Wrote " << ec.message() << std::endl;
        return;
    }

    if (wrote_length < total_length) {
        this->doInnerPhase3Write(offset + wrote_length, total_length - wrote_length);
    } else {
        this->doInnerPhase4();
    }
}

void Session::Private::doInnerPhase4() {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(this->outgoing_buffer);
    auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
        self->_->onInnerPhase4Read(ec, length);
    };
    this->inner_socket.async_read_some(buffer, fn);
}

void Session::Private::onInnerPhase4Read(const ErrorCode & ec, std::size_t length) {
    if (ec) {
        std::cerr << "onInnerPhase4Read " << ec.message() << std::endl;
        return;
    }

    if (this->outgoing_buffer[1] != 0x00) {
        std::cerr << "onInnerPhase4Read server replied error" << std::endl;
        return;
    }

    switch (this->outgoing_buffer[3]) {
    case 0x01:
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    default:
        std::cerr << "onInnerPhase4Read unknown address type" << std::endl;
        return;
    }

    this->doOuterRead();
    this->doInnerRead();
}

void Session::Private::doOuterRead() {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(this->incoming_buffer);
    auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
        self->_->doInnerWrite(0, length);
    };
    this->outer_socket.async_read_some(buffer, fn);
}

void Session::Private::doInnerWrite(std::size_t offset, std::size_t length) {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(&this->incoming_buffer[offset], length);
    auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
        self->_->onInnerWrote(ec, offset, length, wrote_length);
    };
    this->inner_socket.async_write_some(buffer, fn);
}

void Session::Private::onInnerWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
    // TODO handle error
    if (ec) {
        std::cerr << "onInnerWrote " << ec.message() << std::endl;
        return;
    }

    if (wrote_length < total_length) {
        this->doInnerWrite(offset + wrote_length, total_length - wrote_length);
    } else {
        this->doOuterRead();
    }
}

void Session::Private::doInnerRead() {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(this->outgoing_buffer);
    auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
        self->_->doOuterWrite(0, length);
    };
    this->inner_socket.async_read_some(buffer, fn);
}

void Session::Private::doOuterWrite(std::size_t offset, std::size_t length) {
    auto self = this->kungFuDeathGrip();
    auto buffer = boost::asio::buffer(&this->outgoing_buffer[offset], length);
    auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
        self->_->onOuterWrote(ec, offset, length, wrote_length);
    };
    this->outer_socket.async_write_some(buffer, fn);
}

void Session::Private::onOuterWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
    // TODO handle error
    if (ec) {
        std::cerr << "onOuterWrote " << ec.message() << std::endl;
        return;
    }

    if (wrote_length < total_length) {
        this->doOuterWrite(offset + wrote_length, total_length - wrote_length);
    } else {
        this->doInnerRead();
    }
}

std::size_t Session::Private::fillIpv4(Chunk & buffer, std::size_t offset) {
    // ATYP
    buffer[offset++] = 0x01;

    // DST.ADDR
    auto bytes = Application::instance().httpHostAsIpv4().to_bytes();
    std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), offset));

    return 1 + bytes.size();
}

std::size_t Session::Private::fillIpv6(Chunk & buffer, std::size_t offset) {
    // ATYP
    buffer[offset++] = 0x04;

    // DST.ADDR
    auto bytes = Application::instance().httpHostAsIpv6().to_bytes();
    std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), offset));

    return 1 + bytes.size();
}

std::size_t Session::Private::fillFqdn(Chunk & buffer, std::size_t offset) {
    // ATYP
    buffer[offset++] = 0x03;

    // DST.ADDR
    const std::string & hostname = Application::instance().httpHostAsFqdn();
    buffer[offset++] = static_cast<uint8_t>(hostname.size());
    std::copy(std::begin(hostname), std::end(hostname), std::next(std::begin(buffer), offset));

    return 1 + 1 + hostname.size();
}
