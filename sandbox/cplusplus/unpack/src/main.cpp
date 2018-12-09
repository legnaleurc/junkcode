#include "unpack.hpp"

#ifndef _WIN32
#include <locale.h>
#endif


int main (int argc, char * argv[]) {
    if (argc != 4) {
        return 1;
    }

    // libarchive/libarchive#587
#ifndef _WIN32
    setlocale(LC_ALL, "");
#endif

    auto port = std::stoul(argv[1]);
    unpackTo(port, argv[2], argv[3]);

    return 0;
}
