#include "unpack.hpp"


int main (int argc, char * argv[]) {
    if (argc != 3) {
        return 1;
    }

    unpack_to(argv[1], argv[2]);

    return 0;
}
