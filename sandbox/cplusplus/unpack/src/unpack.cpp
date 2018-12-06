#include "unpack.hpp"

#include <archive.h>
#include <cpprest/http_client.h>

#include <memory>
#include <cassert>


struct Context {
    std::string url;
    std::string path;
};


using ContextHandle = std::shared_ptr<Context>;
using ArchiveHandle = std::shared_ptr<struct archive>;


ArchiveHandle createArchiveReader (ContextHandle context);
ArchiveHandle createDiskWriter ();
void extractArchive(ArchiveHandle reader, ArchiveHandle writer);

int open_callback (struct archive * handle, void * context);
int	close_callback (struct archive * handle, void * context);
la_ssize_t read_callback (struct archive * handle, void * context,
                          const void ** buffer);
la_int64_t seek_callback (struct archive * handle, void * context,
                          la_int64_t offset, int whence);


void unpack_to (const std::string & url, const std::string & path) {
    int rv = 0;
    ContextHandle context = std::make_shared<Context>();
    auto reader = createArchiveReader(context);
    auto writer = createDiskWriter();

    for (;;) {
        struct archive_entry * entry = nullptr;
        int rv = archive_read_next_header(reader.get(), &entry);

        if (rv == ARCHIVE_EOF) {
            break;
        }
        assert(rv == ARCHIVE_OK || !"archive_read_next_header");

        rv = archive_write_header(writer.get(), entry);
        assert(rv == ARCHIVE_OK || !"archive_write_header");

        extractArchive(reader, writer);

        rv = archive_write_finish_entry(writer.get());
        assert(rv == ARCHIVE_OK || !"archive_write_finish_entry");
    }
}


ArchiveHandle createArchiveReader (ContextHandle context) {
    ArchiveHandle handle(archive_read_new(), [](ArchiveHandle::element_type * p) -> void {
        archive_read_free(p);
    });

    archive_read_support_filter_all(handle.get());
    archive_read_support_format_all(handle.get());

    archive_read_set_open_callback(handle.get(), open_callback);
    archive_read_set_close_callback(handle.get(), close_callback);
    archive_read_set_read_callback(handle.get(), read_callback);
    archive_read_set_seek_callback(handle.get(), seek_callback);

    archive_read_set_callback_data(handle.get(), context.get());

    return handle;
}


ArchiveHandle createDiskWriter () {
    ArchiveHandle handle(archive_write_disk_new(), [](ArchiveHandle::element_type * p) -> void {
        archive_write_free(p);
    });

    return handle;
}
