#include "cppshell.hpp"

#include <iostream>


int main (int argc, char * * argv) {
    cppshell::Command ls("ls");
    cppshell::Command grep("grep");
    cppshell::Command sort("sort");

    // auto s = (ls % "-lh" % "/tmp")();
    // auto s = (ls[{"-lh", "/tmp"}] | grep[{"a"}] | sort) ();
    auto s = (ls[{"-lh", "/tmp"}] | grep[{"a"}]) ();
    // auto s = (ls[{"-lh", "/tmp"}]) ();
    // ls[{"-lh", "/tmp", "/"}];

    std::cout << "stdout :" << std::endl;
    std::cout << s << std::endl;

    return 0;
}
