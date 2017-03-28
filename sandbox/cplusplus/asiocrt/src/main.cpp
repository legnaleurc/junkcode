#include "coroutine.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

int main(int argc, char * argv[]) {
    boost::asio::io_service loop;

    crt::spawn(loop, [&loop](crt::YieldContext yield) -> void {
        boost::asio::ip::tcp::resolver resolver(loop);
        auto it = resolver.async_resolve({
            "www.google.com",
            80,
        }, yield);
    });

    return 0;
}
