#ifndef SESSION_HPP
#define SESSION_HPP

#include "global.hpp"

#include <memory>


namespace s5p {

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(Socket socket);

    void start();

private:
    Session(const Session &);
    Session & operator = (const Session &);
    Session(Session &&);
    Session & operator = (Session &&);

    class Private;
    std::shared_ptr<Private> _;
};

}

#endif
