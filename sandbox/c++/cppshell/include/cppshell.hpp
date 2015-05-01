#ifndef CPPSHELL_HPP
#define CPPSHELL_HPP

#include <memory>
#include <string>
#include <initializer_list>


namespace cppshell {

class Command {
public:
    explicit Command (const char * cmd);
    explicit Command (const std::string & cmd);

    Command & operator [] (const std::initializer_list<std::string> & args);
    std::string operator ()() const;
    Command & operator | (const Command & command);
    // Command operator | (const char * cmd) const;
    // Command operator | (const std::string & cmd) const;

private:
    Command (const Command &);
    Command & operator = (const Command &);

    class Private;
    friend class Private;
    std::shared_ptr<Private> d;
};

}

#endif
