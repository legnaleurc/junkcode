#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "session.hpp"

#include <memory>


namespace s5p {

class Session::Private {
public:
    Private(Socket socket);

    std::shared_ptr<Session> kungFuDeathGrip();

    void doInnerResolve();
    void onResolved(const ErrorCode & ec, Resolver::iterator it);
    void doInnerConnect(Resolver::iterator it);
    void onInnerConnected(const ErrorCode & ec, Resolver::iterator it);
    void doInnerPhase1();
    void doInnerPhase1Write(std::size_t offset, std::size_t length);
    void onInnerPhase1Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length);
    void doInnerPhase2();
    void onInnerPhase2Read(const ErrorCode & ec, std::size_t length);
    void doInnerPhase3();
    void doInnerPhase3Write(std::size_t offset, std::size_t length);
    void onInnerPhase3Wrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length);
    void doInnerPhase4();
    void onInnerPhase4Read(const ErrorCode & ec, std::size_t length);
    void doOuterRead();
    void doInnerWrite(std::size_t offset, std::size_t length);
    void onInnerWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length);
    void doInnerRead();
    void doOuterWrite(std::size_t offset, std::size_t length);
    void onOuterWrote(const ErrorCode & ec, std::size_t offset, std::size_t total_length, std::size_t wrote_length);
    std::size_t fillIpv4(Chunk & buffer, std::size_t offset);
    std::size_t fillIpv6(Chunk & buffer, std::size_t offset);
    std::size_t fillFqdn(Chunk & buffer, std::size_t offset);

    std::weak_ptr<Session> self;
    Socket outer_socket;
    Socket inner_socket;
    Chunk incoming_buffer;
    Chunk outgoing_buffer;
    Resolver resolver;
};

}

#endif
