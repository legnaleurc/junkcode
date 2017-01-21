#include <boost/asio.hpp>

#include <csignal>
#include <iostream>


typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::ip::tcp::resolver Resolver;
typedef boost::system::error_code ErrorCode;
typedef std::vector<uint8_t> Chunk;


const static std::size_t CHUNK_SIZE = 8192;


Chunk createChunk() {
    return Chunk(CHUNK_SIZE);
}


class Session : public std::enable_shared_from_this<Session> {
public:
    Session(Socket socket)
        : outer_socket_(std::move(socket))
        , inner_socket_(outer_socket_.get_io_service())
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
        
        Resolver::query q("localhost", "7000");

        this->resolver_.async_resolve(q, fn);
    }

    void onResolved(const ErrorCode & ec, Resolver::iterator it) {
        // TODO handle error
        if (ec || Resolver::iterator() == it) {
            std::cerr << "onResolved failed" << ec << std::endl;
            return;
        }

        this->doInnerConnect(it);
    }

    void doInnerConnect(Resolver::iterator it) {
        auto self = this->shared_from_this();
        auto fn = [self, it](const ErrorCode & ec) -> void {
            self->onInnerConnected(ec, std::next(it));
        };

        this->inner_socket_.async_connect(*it++, fn);
    }

    void onInnerConnected(const ErrorCode & ec, Resolver::iterator it) {
        if (ec) {
            if (Resolver::iterator() == it) {
                std::cerr << "onInnerConnected error" << ec << std::endl;
                return;
            }

            this->doInnerConnect(it);
            return;
        }

        auto self = this->shared_from_this();
        
        this->doOuterRead();
        this->doInnerRead();
    }

    void doOuterRead() {
        auto self = this->shared_from_this();
        Chunk chunk = createChunk();
        auto buffer = boost::asio::buffer(chunk);
        auto fn = [self, &chunk](const ErrorCode & ec, std::size_t length) -> void {
            self->doInnerWrite(chunk, 0, length);
        };
        this->outer_socket_.async_read_some(buffer, fn);
    }

    void doInnerWrite(const Chunk & chunk, std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&chunk[offset], length);
        auto fn = [self, &chunk, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onInnerWrote(ec, chunk, offset, length, wrote_length);
        };
        this->inner_socket_.async_write_some(buffer, fn);
    }

    void onInnerWrote(const ErrorCode & ec, const Chunk & chunk, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

        // TODO handle error
        if (!ec) {
            if (wrote_length < total_length) {
                this->doInnerWrite(chunk, offset + wrote_length, total_length - wrote_length);
            } else {
                this->doOuterRead();
            }
        }
    }

    void doInnerRead() {
        auto self = this->shared_from_this();
        Chunk chunk = createChunk();
        auto buffer = boost::asio::buffer(chunk);
        auto fn = [self, &chunk](const ErrorCode & ec, std::size_t length) -> void {
            self->doOuterWrite(chunk, 0, length);
        };
        this->inner_socket_.async_read_some(buffer, fn);
    }

    void doOuterWrite(const Chunk & chunk, std::size_t offset, std::size_t length) {
        auto self = this->shared_from_this();
        auto buffer = boost::asio::buffer(&chunk[offset], length);
        auto fn = [self, &chunk, offset, length](const ErrorCode & ec, std::size_t wrote_length) -> void {
            self->onOuterWrote(ec, chunk, offset, length, wrote_length);
        };
        this->outer_socket_.async_write_some(buffer, fn);
    }

    void onOuterWrote(const ErrorCode & ec, const Chunk & chunk, std::size_t offset, std::size_t total_length, std::size_t wrote_length) {
        auto self = this->shared_from_this();

        // TODO handle error
        if (!ec) {
            if (wrote_length < total_length) {
                this->doOuterWrite(chunk, offset + wrote_length, total_length - wrote_length);
            } else {
                this->doInnerRead();
            }
        }
    }

    Socket outer_socket_;
    Socket inner_socket_;
    Resolver resolver_;
};


class Server {
public:
    Server(boost::asio::io_service & loop, unsigned short port)
        : acceptor_(loop, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        , socket_(loop)
    {
    }

    void start() {
        this->doAccept();
    }

private:
    void doAccept() {
        this->acceptor_.async_accept(this->socket_, [this](boost::system::error_code ec) -> void {
            if (!ec) {
                std::make_shared<Session>(std::move(this->socket_))->start();
            }

            this->doAccept();
        });
    }

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};


int main(int argc, char * argv[]) {
    boost::asio::io_service loop;

    Server server(loop, 8000);
    server.start();

    loop.run();

    return 0;
}

