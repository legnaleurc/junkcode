#include "cppshell.hpp"

#include <iostream>
#include <exception>
#include <stack>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>


namespace {

struct Unit {
    std::string cmd;
    std::vector<std::string> args;

    char * * c_args () const {
        char * * args_ = new char*[this->args.size() + 1];
        std::size_t i;
        for (i = 0; i < this->args.size(); ++i) {
            args_[i] = const_cast<char *>(this->args[i].c_str());
        }
        args_[i] = NULL;
        return args_;
    }
};

}


namespace cppshell {

class Command::Private {
public:
    Private (const std::string & cmd);

    void pipeline (int wfd);

    std::stack<Unit> command_stack;
};

}


using cppshell::Command;


Command::Command (const char * cmd):
d(new Private(cmd)) {
}

Command::Command (const std::string & cmd):
d(new Private(cmd)) {
}

Command & Command::operator [](const std::initializer_list<std::string> & args) {
    auto & unit = this->d->command_stack.top();
    unit.args = args;
    return *this;
}

std::string Command::operator ()() const {
    // create pipes
    int fds[2];
    if (pipe(fds) < 0) {
        throw std::logic_error("pipe error");
    }

    // fork child
    pid_t pid = fork();
    if (pid < 0) {
        throw std::logic_error("fork error");
    } else if (pid > 0) {
        // parent
        wait(NULL);
        close(fds[1]);
        char buffer[8192] = "";
        ssize_t ret = read(fds[0], buffer, sizeof(buffer));
        return buffer;
    } else {
        // child
        this->d->pipeline(fds[1]);
    }
    return "";
}

Command & Command::operator | (const Command & command) {
    Unit unit = command.d->command_stack.top();
    this->d->command_stack.push(unit);
    return *this;
}


Command::Private::Private (const std::string & cmd):
command_stack() {
    Unit unit;
    unit.cmd = cmd;
    this->command_stack.push(unit);
}

void Command::Private::pipeline (int wfd) {
    // pop a level
    Unit unit = this->command_stack.top();
    this->command_stack.pop();

    if (this->command_stack.empty()) {
        std::cerr << "cmd: " << unit.cmd << " " << wfd << std::endl;
        // latest level, stop recursion
        dup2(wfd, STDOUT_FILENO);
        execvp(unit.cmd.c_str(), unit.c_args());
        return;
    }

    // create pipes
    int fds[2];
    if (pipe(fds) < 0) {
        throw std::logic_error("pipe error");
    }
    std::cerr << "cmd: " << unit.cmd << " " << wfd << " " << fds[0] << " " << fds[1] << std::endl;
    // fork child
    pid_t pid = fork();
    if (pid < 0) {
        throw std::logic_error("fork error");
    } else if (pid > 0) {
        // parent process
        wait(NULL);
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        dup2(wfd, STDOUT_FILENO);
        execvp(unit.cmd.c_str(), unit.c_args());
    } else {
        // child process
        close(fds[0]);
        // dup2(fds[0], STDIN_FILENO);
        // dup2(fds[1], STDOUT_FILENO);
        this->pipeline(fds[1]);
    }
}
