#ifndef COROUTINE_HPP
#define COROUTINE_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/coroutine2/coroutine.hpp>

namespace crt {

struct YieldContext {
    typedef boost::coroutines2::coroutine<boost::asio::ip::tcp::resolver::iterator>::push_type CalleeType;
    typedef boost::coroutines2::coroutine<boost::asio::ip::tcp::resolver::iterator>::pull_type CallerType;

    YieldContext() {}

    std::shared_ptr<CalleeType> callee;
    std::shared_ptr<CallerType> caller;
};

void spawn(boost::asio::io_service & loop, std::function<void (YieldContext)> fn);

template<typename ReturnType>
class CoroutineHandler {
public:
    explicit CoroutineHandler (crt::YieldContext & yield)
        : yield(yield)
    {}

    void operator () (boost::system::error_code ec, ReturnType rv) {
        auto & callee = *yield.callee;
        callee(rv);
    }

    ReturnType get() const {
        auto & caller = *yield.caller;
        return caller().get();
    }

private:
    crt::YieldContext & yield;
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

    async_result(crt::CoroutineHandler<ReturnType> & handler)
        : h_(handler)
    {}

    type get() {
        return h_.get();
    }

private:
    crt::CoroutineHandler<ReturnType> & h_;
};

}
}

#endif
