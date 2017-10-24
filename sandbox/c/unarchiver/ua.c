#include <archive.h>
#include <archive_entry.h>

#include <stdio.h>
#include <assert.h>

#include <locale.h>


int list_archive(const char * file_name);


int main (int argc, char * argv[]) {
    if (argc != 2) {
        return 1;
    }
    // TODO Windows?
    setlocale(LC_ALL, "");
    int rv = 0;
    rv = list_archive(argv[1]);
    return 0;
}


int list_archive(const char * file_name) {
    int rv = 0;
    struct archive * handle = archive_read_new();
    assert(handle || !"null archive handle");

    rv = archive_read_support_format_all(handle);
    assert(rv == ARCHIVE_OK || !"no supported format");
    rv = archive_read_support_filter_all(handle);
    assert(rv == ARCHIVE_OK || !"no supported filter");

    rv = archive_read_open_filename(handle, file_name, 1024);
    if (rv != ARCHIVE_OK) {
        printf("archive_read_open_filename: %s (%d)\n",
               archive_error_string(handle), rv);
        return 1;
    }

    for (;;) {
        struct archive_entry * entry = NULL;
        rv = archive_read_next_header(handle, &entry);

        if (rv == ARCHIVE_EOF) {
            break;
        }

        if (rv != ARCHIVE_OK) {
            printf("archive_read_next_header: %s (%d)\n",
                   archive_error_string(handle), rv);
            if (rv != ARCHIVE_WARN) {
                assert(!"broken entry?");
                break;
            }
        }

        const char * entry_path = archive_entry_pathname_utf8(entry);
        assert(entry_path || !"null entry path");

        printf("%s\n", entry_path);
    }

finish:
    rv = archive_read_free(handle);
    assert(rv == 0 || !"archive handle free error");
    return rv;
}
