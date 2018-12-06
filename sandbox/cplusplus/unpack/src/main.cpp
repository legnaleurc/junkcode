#include "unpack.hpp"


int main (int argc, char * argv[]) {
    if (argc != 4) {
        return 1;
    }

    auto port = std::stoul(argv[1]);
    unpackTo(port, argv[2], argv[3]);

    return 0;
}
