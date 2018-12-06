#include "unpack.h"

#include <archive.h>


int unpack_to(int port, const char * http_path, const char * output_path) {
    int rv = 0;
    struct archive * handle = archive_read_new();
    rv = archive_read_free(handle);
    return rv;
}
