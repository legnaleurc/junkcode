#ifndef COROUTINE_HPP
#define COROUTINE_HPP

#include <boost/asio/io_service.hpp>

namespace crt {

template<typename Function>
void spawn(boost::asio::io_service & loop, Function fn);

}

#endif
