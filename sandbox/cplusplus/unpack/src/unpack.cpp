#include "unpack.hpp"

#include <archive.h>


void unpack_to(const std::string & url, const std::string & path) {
    int rv = 0;
    struct archive * handle = archive_read_new();
    rv = archive_read_free(handle);
    return rv;
}
