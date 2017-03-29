#ifndef COROUTINE_HPP
#define COROUTINE_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/coroutine2/coroutine.hpp>

namespace crt {

struct YieldContext {
    typedef boost::coroutines2::coroutine<void>::push_type CalleeType;
    typedef boost::coroutines2::coroutine<void>::pull_type CallerType;
};

void spawn(boost::asio::io_service & loop, std::function<void (YieldContext)> fn);

template<typename T>
class CoroutineHandler {
public:
    explicit CoroutineHandler (crt::YieldContext &);

    void operator () (boost::system::error_code, boost::asio::ip::tcp::resolver::iterator);
};

}

namespace boost {
namespace asio {

template<typename ReturnType>
struct handler_type<crt::YieldContext, void (boost::system::error_code, ReturnType)> {
    typedef crt::CoroutineHandler<ReturnType> type;
};

template<typename ReturnType>
class async_result<crt::CoroutineHandler<ReturnType>> {
public:
    typedef ReturnType type;

    async_result(crt::CoroutineHandler<ReturnType> & handler);

    type get();
};

}
}

#endif
