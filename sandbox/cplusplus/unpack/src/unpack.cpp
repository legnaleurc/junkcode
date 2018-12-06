#include "unpack.hpp"

#include <archive.h>
#include <cpprest/http_client.h>

#include <memory>
#include <cassert>


using ArchiveHandle = std::shared_ptr<struct archive>;


ArchiveHandle createArchiveHandle ();

int open_callback (struct archive * handle, void * context);
int	close_callback (struct archive * handle, void * context);
la_ssize_t read_callback (struct archive * handle, void * context,
                          const void ** buffer);
la_int64_t seek_callback (struct archive * handle, void * context,
                          la_int64_t offset, int whence);


void unpack_to (const std::string & url, const std::string & path) {
    int rv = 0;
    auto handle = createArchiveHandle();
}


ArchiveHandle createArchiveHandle () {
    ArchiveHandle handle(archive_read_new(), [](ArchiveHandle::element_type * p) -> void {
        archive_read_free(p);
    });

    archive_read_support_filter_all(handle.get());
    archive_read_support_format_all(handle.get());

    return handle;
}
