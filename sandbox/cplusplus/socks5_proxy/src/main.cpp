#include <boost/asio.hpp>

#include <cassert>
#include <csignal>
#include <iostream>

#if !defined(__APPLE__) && !defined(_WIN32)
#include <endian.h>
#else
#include <boost/endian/conversion.hpp>
#endif


const static std::size_t CHUNK_SIZE = 8192;


typedef boost::asio::ip::tcp::acceptor Acceptor;
typedef boost::asio::ip::tcp::endpoint EndPoint;
typedef boost::asio::ip::tcp::resolver Resolver;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::system::error_code ErrorCode;
typedef std::array<uint8_t, CHUNK_SIZE> Chunk;


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


class Session : public std::enable_shared_from_this<Session> {
public:
    Session(Socket socket)
        : outer_socket_(std::move(socket))
        , inner_socket_(outer_socket_.get_io_service())
        , incoming_buffer_(createChunk())
        , outgoing_buffer_(createChunk())
        , resolver_(outer_socket_.get_io_service())
    {
    }

    void start() {
        this->doInnerResolve();
    }

private:
    void doInnerResolve() {
        auto self = this->shared_from_this();
        auto fn = [self](const ErrorCode & ec, Resolver::iterator it) -> void {
            self->onResolved(ec, it);
        };

        Resolver::query q("localhost", "1080");

        this->resolver_.async_resolve(q, fn);
    }

    void onResolved(const ErrorCode & ec, Resolver::iterator it) {
        // TODO handle error
        if (ec) {
            std::cerr << "onResolved failed" << ec << std::endl;
            return;
        }

        this->doInnerConnect(it);
    }

    void doInnerConnect(Resolver::iterator it) {
        if (Resolver::iterator() == it) {
            return;
        }

        auto self = this->shared_from_this();
        auto fn = [self, it](const ErrorCode & ec) -> void {
            self->onInnerConnected(ec, it);
        };

        this->inner_socket_.async_connect(*it, fn);
    }

    void onInnerConnected(const ErrorCode & ec, Resolver::iterator it) {
        if (ec) {
            std::cerr << "onInnerConnected failed " << ec.message() << std::endl;
            this->inner_socket_.close();
            it = std::next(it);
            this->doInnerConnect(it);
            return;
        }

        this->doInnerPhase1();

        // this->doOuterRead();
        // this->doInnerRead();
    }

    void doInnerPhase1() {
        auto & buffer = this->incoming_buffer_;
        // VER
        buffer[0] = 0x05;
        // NMETHODS
        buffer[1] = 0x01;
        // METHODS
        buffer[2] = 0x00;

        this->doInnerPhase1Write(0, 3);
    }

    void doInnerPhase1Write(std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&this->incoming_buffer_[offset], length);
        auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onInnerPhase1Wrote(ec, offset, length, wrote_length);
        };
        this->inner_socket_.async_write_some(buffer, fn);
    }

    void onInnerPhase1Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

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

    void doInnerPhase2() {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(this->outgoing_buffer_);
        auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
            self->onInnerPhase2Read(ec, length);
        };
        this->inner_socket_.async_read_some(buffer, fn);
    }

    void onInnerPhase2Read(const ErrorCode & ec, std::size_t length) {
        if (ec) {
            std::cerr << "onInnerPhase2Read " << ec.message() << std::endl;
            return;
        }

        if (length < 2) {
            std::cerr << "onInnerPhase2Read wrong auth header length" << std::endl;
            return;
        }

        if (this->outgoing_buffer_[1] != 0x00) {
            std::cerr << "onInnerPhase2Read provided auth not supported" << std::endl;
            return;
        }

        this->doInnerPhase3();
    }

    void doInnerPhase3() {
        auto & buffer = this->incoming_buffer_;
        // VER
        buffer[0] = 0x05;
        // CMD
        buffer[1] = 0x01;
        // RSV
        buffer[2] = 0x00;

        // TODO parse real header

#if 0
        // ATYP
        buffer[3] = 0x03;

        // DST.ADDR
        std::string hostname = "acddl.loc.al";
        buffer[4] = static_cast<uint8_t>(hostname.size());
        std::copy(std::begin(hostname), std::end(hostname), std::next(std::begin(buffer), 5));

        // DST.PORT
        putBigEndian(&buffer[4 + 1 + hostname.size()], 80);

        std::size_t total_length = 4 + 1 + hostname.size() + 2;
#endif

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

    void doInnerPhase3Write(std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&this->incoming_buffer_[offset], length);
        auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onInnerPhase3Wrote(ec, offset, length, wrote_length);
        };
        this->inner_socket_.async_write_some(buffer, fn);
    }

    void onInnerPhase3Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

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

    void doInnerPhase4() {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(this->outgoing_buffer_);
        auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
            self->onInnerPhase4Read(ec, length);
        };
        this->inner_socket_.async_read_some(buffer, fn);
    }

    void onInnerPhase4Read(const ErrorCode & ec, std::size_t length) {
        if (ec) {
            std::cerr << "onInnerPhase4Read " << ec.message() << std::endl;
            return;
        }

        if (this->outgoing_buffer_[1] != 0x00) {
            std::cerr << "onInnerPhase4Read server replied error" << std::endl;
            return;
        }

        switch (this->outgoing_buffer_[3]) {
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

    void doOuterRead() {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(this->incoming_buffer_);
        auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
            self->doInnerWrite(0, length);
        };
        this->outer_socket_.async_read_some(buffer, fn);
    }

    void doInnerWrite(std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&this->incoming_buffer_[offset], length);
        auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onInnerWrote(ec, offset, length, wrote_length);
        };
        this->inner_socket_.async_write_some(buffer, fn);
    }

    void onInnerWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

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

    void doInnerRead() {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(this->outgoing_buffer_);
        auto fn = [self](const ErrorCode & ec, std::size_t length) -> void {
            self->doOuterWrite(0, length);
        };
        this->inner_socket_.async_read_some(buffer, fn);
    }

    void doOuterWrite(std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&this->outgoing_buffer_[offset], length);
        auto fn = [self, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onOuterWrote(ec, offset, length, wrote_length);
        };
        this->outer_socket_.async_write_some(buffer, fn);
    }

    void onOuterWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

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

    Socket outer_socket_;
    Socket inner_socket_;
    Chunk incoming_buffer_;
    Chunk outgoing_buffer_;
    Resolver resolver_;
};


class Server {
public:
    Server(boost::asio::io_service & loop, unsigned short port)
        : v4_acceptor_(loop, EndPoint(boost::asio::ip::tcp::v4(), port))
        , v6_acceptor_(loop, EndPoint(boost::asio::ip::tcp::v6(), port))
        , socket_(loop)
    {
    }

    void start() {
        this->doV4Accept();
        this->doV6Accept();
    }

private:
    void doV4Accept() {
        this->v4_acceptor_.async_accept(this->socket_, [this](const ErrorCode & ec) -> void {
            if (!ec) {
                std::make_shared<Session>(std::move(this->socket_))->start();
            }

            this->doV4Accept();
        });
    }

    void doV6Accept() {
        this->v6_acceptor_.async_accept(this->socket_, [this](const ErrorCode & ec) -> void {
            if (!ec) {
                std::make_shared<Session>(std::move(this->socket_))->start();
            }

            this->doV6Accept();
        });
    }

    Acceptor v4_acceptor_;
    Acceptor v6_acceptor_;
    Socket socket_;
};


int main(int argc, char * argv[]) {
    boost::asio::io_service loop;

    boost::asio::signal_set signals(loop, SIGINT);

    Server server(loop, 8000);

    signals.async_wait([&loop](const ErrorCode & ec, int signal_number) -> void {
        if (ec) {
            // TODO handle error
        }
        loop.stop();
    });

    server.start();

    loop.run();

    return 0;
}
