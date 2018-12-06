#include "unpack.hpp"

#include <archive.h>
#include <cpprest/http_client.h>
#include <cpprest/rawptrstream.h>

#include <memory>
#include <cassert>


class Context {
public:
    Context (const std::string & url);

    web::http::http_response & getResponse ();
    bool seek (int64_t offset, int whence);
    void reset ();

private:
    std::string url;
    web::http::http_response response;
    int64_t offset;
    int64_t length;
};


using ContextHandle = std::shared_ptr<Context>;
using ArchiveHandle = std::shared_ptr<struct archive>;


ArchiveHandle createArchiveReader (ContextHandle context);
ArchiveHandle createDiskWriter ();
void extractArchive (ArchiveHandle reader, ArchiveHandle writer);

int openCallback (struct archive * handle, void * context);
int	closeCallback (struct archive * handle, void * context);
la_ssize_t readCallback (struct archive * handle, void * context,
                         const void ** buffer);
la_int64_t seekCallback (struct archive * handle, void * context,
                         la_int64_t offset, int whence);


void unpack_to (const std::string & url, const std::string & path) {
    ContextHandle context = std::make_shared<Context>(url);
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
    int rv = 0;

    ArchiveHandle handle(archive_read_new(), [](ArchiveHandle::element_type * p) -> void {
        archive_read_free(p);
    });

    rv = archive_read_support_filter_all(handle.get());
    assert(rv == ARCHIVE_OK || !"archive_read_support_filter_all");
    rv = archive_read_support_format_all(handle.get());
    assert(rv == ARCHIVE_OK || !"archive_read_support_format_all");

    rv = archive_read_set_open_callback(handle.get(), openCallback);
    assert(rv == ARCHIVE_OK || !"archive_read_set_open_callback");
    rv = archive_read_set_close_callback(handle.get(), closeCallback);
    assert(rv == ARCHIVE_OK || !"archive_read_set_close_callback");
    rv = archive_read_set_read_callback(handle.get(), readCallback);
    assert(rv == ARCHIVE_OK || !"archive_read_set_read_callback");
    rv = archive_read_set_seek_callback(handle.get(), seekCallback);
    assert(rv == ARCHIVE_OK || !"archive_read_set_seek_callback");

    rv = archive_read_set_callback_data(handle.get(), context.get());
    assert(rv == ARCHIVE_OK || !"archive_read_set_callback_data");

    rv = archive_read_open1(handle.get());
    assert(rv == ARCHIVE_OK || !"archive_read_open1");

    return handle;
}


ArchiveHandle createDiskWriter () {
    ArchiveHandle handle(archive_write_disk_new(), [](ArchiveHandle::element_type * p) -> void {
        archive_write_free(p);
    });

    return handle;
}


void extractArchive (ArchiveHandle reader, ArchiveHandle writer) {
    for (;;) {
        int rv = 0;
        const void * chunk = nullptr;
        size_t length = 0;
        la_int64_t offset = 0;

        rv = archive_read_data_block(reader.get(), &chunk, &length, &offset);
        if (rv == ARCHIVE_EOF) {
            break;
        }
        assert(rv == ARCHIVE_OK || !"archive_read_data_block");

        rv = archive_write_data_block(writer.get(), chunk, length, offset);
        assert(rv == ARCHIVE_OK || !"archive_write_data_block");
    }
}


int openCallback (struct archive * handle, void * context) {
    auto ctx = static_cast<Context *>(context);
    ctx->reset();
    return ARCHIVE_OK;
}


int	closeCallback (struct archive * handle, void * context) {
    auto ctx = static_cast<Context *>(context);
    ctx->reset();
    return ARCHIVE_OK;
}


la_ssize_t readCallback (struct archive * handle, void * context,
                         const void ** buffer)
{
    auto ctx = static_cast<Context *>(context);
    auto & response = ctx->getResponse();
    *buffer = malloc(65536);
    Concurrency::streams::rawptr_buffer<uint8_t> chunk(static_cast<const uint8_t *>(*buffer), 65536);
    auto length = response.body().read(chunk, 65536).get();
    return length;
}


la_int64_t seekCallback (struct archive * handle, void * context,
                         la_int64_t offset, int whence)
{
    auto ctx = static_cast<Context *>(context);
    auto ok = ctx->seek(offset, whence);
    return ok ? offset : ARCHIVE_FATAL;
}


Context::Context (const std::string & url)
    : url(url)
    , response()
    , offset(0)
    , length(-1)
{}


web::http::http_response &
Context::getResponse () {
    auto status = this->response.status_code();
    if (status == web::http::status_codes::OK) {
        return this->response;
    }

    web::http::http_request request;
    request.set_method(web::http::methods::GET);
    request.set_request_uri(this->url);
    if (this->length >= 0) {
        request.headers().add("Range", "");
    }

    web::http::client::http_client client(this->url);
    this->response = client.request(request).get();
    this->length = this->response.headers().content_length();
    return this->response;
}


bool Context::seek (int64_t offset, int whence) {
    this->response = web::http::http_response();

    switch (whence) {
    case SEEK_SET:
        this->offset = offset;
    case SEEK_CUR:
        this->offset += offset;
    case SEEK_END:
        if (this->length < 0) {
            return false;
        }
        this->offset = this->length + offset;
    default:
        return false;
    }

    return this->offset >= 0;
}


void Context::reset () {
    this->response = web::http::http_response();
    this->offset = 0;
    this->length = -1;
}
