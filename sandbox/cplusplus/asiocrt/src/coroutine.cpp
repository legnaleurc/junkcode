#include "coroutine.hpp"

namespace crt {

void spawn(boost::asio::io_service & loop, std::function<void (YieldContext)> fn) {
    YieldContext yield;
    loop.post(std::bind(fn, yield));
}

}
