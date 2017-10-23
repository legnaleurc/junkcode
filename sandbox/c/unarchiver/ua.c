#include <archive.h>
#include <archive_entry.h>

#include <stdio.h>
#include <assert.h>


int list_archive(const char * file_name);


int main (int argc, char * argv[]) {
    int rv = 0;
    rv = list_archive("test.zip");
    return 0;
}


int list_archive(const char * file_name) {
    int rv = 0;
    struct archive * handle = archive_read_new();
    assert(handle || !"null archive handle");

    rv = archive_read_support_format_all(handle);
    assert(rv == 0 || !"no supported format");

    rv = archive_read_open_filename(handle, file_name, 1024);
    if (rv != 0) {
        printf("archive_read_open_filename: %s (%d)\n",
               archive_error_string(handle), rv);
        return 1;
    }

    rv = archive_read_free(handle);
    assert(rv == 0 || !"archive handle free error");
    return rv;
}
