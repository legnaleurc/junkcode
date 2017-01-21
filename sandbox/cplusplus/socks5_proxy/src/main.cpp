#include <boost/asio.hpp>

#include <csignal>


class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket))
        , inner_socket_(socket_.get_io_service())
    {
    }

    void start() {
        this->doRead();
    }

private:
    void doRead() {
        auto self = this->shared_from_this();
        this->socket_.async_read_some(boost::asio::buffer(this->buffer_, sizeof(this->buffer_)), [self](boost::system::error_code ec, std::size_t length) -> void {
            if (!ec) {
                self->doWrite(length);
            }
        });
    }

    void doWriteInner(std::size_t length) {
        auto self = this->shared_from_this();
        char buffer[8092];
        this->socket_.async_write_some(boost::asio::buffer(this->buffer_, sizeof(this->buffer_)), [self](boost::system::error_code ec, std::size_t length) -> void {
            if (!ec) {
                self->doRead();
            }
        });
    }

    void doWrite(std::size_t length) {
        auto self = this->shared_from_this();
        char buffer[8092];
        this->socket_.async_write_some(boost::asio::buffer(this->buffer_, sizeof(this->buffer_)), [self](boost::system::error_code ec, std::size_t length) -> void {
            if (!ec) {
                self->doRead();
            }
        });
    }

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::socket inner_socket_;
    char buffer_[8092];
};


class Server {
public:
    Server(boost::asio::io_service & loop, unsigned short port)
        : acceptor_(loop, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        , socket_(loop)
    {
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

    loop.run();

    return 0;
}

