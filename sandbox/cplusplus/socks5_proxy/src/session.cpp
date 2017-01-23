#include "session_p.hpp"

#include "global.hpp"

#include <iostream>


using s5p::Session;


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

    Resolver::query q("localhost", "1080");

    this->resolver.async_resolve(q, fn);
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

    // TODO parse real header

// #if 0
    // ATYP
    buffer[3] = 0x03;

    // DST.ADDR
    std::string hostname = "www.example.org";
    buffer[4] = static_cast<uint8_t>(hostname.size());
    std::copy(std::begin(hostname), std::end(hostname), std::next(std::begin(buffer), 5));

    // DST.PORT
    putBigEndian(&buffer[4 + 1 + hostname.size()], 80);

    std::size_t total_length = 4 + 1 + hostname.size() + 2;
// #endif

#if 0
    // ATYP
    buffer[3] = 0x01;

    // DST.ADDR
    buffer[4] = 127;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 1;

    // DST.PORT
    putBigEndian(&buffer[8], 1234);

    std::size_t total_length = 10;
#endif

#if 0
    auto ep = this->inner_socket_.remote_endpoint();
    auto address = ep.address();
    std::size_t total_length = 0;
    if (address.is_v4()) {
        // ATYP
        buffer[3] = 0x01;

        auto bytes = address.to_v4().to_bytes();

        // DST.ADDR
        std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), 4));

        // DST.PORT
        uint16_t * port = reinterpret_cast<uint16_t *>(&buffer[8]);
        *port = boost::endian::native_to_big(ep.port());

        total_length = 10;
    } else {
        // ATYP
        buffer[3] = 0x04;

        auto bytes = address.to_v6().to_bytes();

        // DST.ADDR
        std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), 4));

        // DST.PORT
        uint16_t * port = reinterpret_cast<uint16_t *>(&buffer[20]);
        *port = boost::endian::native_to_big(ep.port());

        total_length = 22;
    }
#endif

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
