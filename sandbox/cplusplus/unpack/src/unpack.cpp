#include "unpack.hpp"

#include <memory>
#include <iomanip>

#include <archive.h>
#include <archive_entry.h>
#include <cpprest/http_client.h>
#include <cpprest/rawptrstream.h>
#include <boost/filesystem.hpp>

#include "types.hpp"
#include "exception.hpp"


const uint64_t CHUNK_SIZE = 65536;


class Context {
public:
    Context (uint16_t port, const std::string & id);

    web::http::http_response & getResponse ();
    int64_t readChunk (const void ** buffer);
    bool seek (int64_t offset, int whence);
    void reset ();

private:
    web::uri base;
    web::uri path;
    web::http::http_response response;
    int64_t offset;
    int64_t length;
    std::array<uint8_t, CHUNK_SIZE> chunk;
};
using ContextHandle = std::shared_ptr<Context>;


ArchiveHandle createArchiveReader (ContextHandle context);
ArchiveHandle createDiskWriter ();
std::string resolvePath (const std::string & localPath, const std::string & id,
                         const std::string & entryName);
void extractArchive (ArchiveHandle reader, ArchiveHandle writer);
web::uri makeBase (uint16_t port);
web::uri makePath (const std::string & id);

int openCallback (struct archive * handle, void * context);
int	closeCallback (struct archive * handle, void * context);
la_ssize_t readCallback (struct archive * handle, void * context,
                         const void ** buffer);
la_int64_t seekCallback (struct archive * handle, void * context,
                         la_int64_t offset, int whence);


void
unpackTo (uint16_t port, const std::string & id,
          const std::string & localPath)
{
    ContextHandle context = std::make_shared<Context>(port, id);
    auto reader = createArchiveReader(context);
    auto writer = createDiskWriter();

    for (;;) {
        struct archive_entry * entry = nullptr;
        int rv = archive_read_next_header(reader.get(), &entry);
        if (rv == ARCHIVE_EOF) {
            break;
        }
        if (rv != ARCHIVE_OK) {
            throw ArchiveError(reader, "archive_read_next_header");
        }

        const char * entryName = archive_entry_pathname(entry);
        if (!entryName) {
            throw new EntryError("archive_entry_pathname", "nullptr");
        }

        auto entryPath = resolvePath(localPath, id, entryName);
        rv = archive_entry_update_pathname_utf8(entry, entryPath.c_str());
        if (!rv) {
            throw new EntryError("archive_entry_update_pathname_utf8", entryPath);
        }

        rv = archive_write_header(writer.get(), entry);
        if (rv != ARCHIVE_OK) {
            throw ArchiveError(writer, "archive_write_header");
        }

        extractArchive(reader, writer);

        rv = archive_write_finish_entry(writer.get());
        if (rv != ARCHIVE_OK) {
            throw ArchiveError(writer, "archive_write_finish_entry");
        }
    }
}


ArchiveHandle createArchiveReader (ContextHandle context) {
    int rv = 0;

    ArchiveHandle handle(
        archive_read_new(),
        [](ArchiveHandle::element_type * p) -> void {
            archive_read_free(p);
        });

    rv = archive_read_support_filter_all(handle.get());
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_support_filter_all");
    }
    rv = archive_read_support_format_all(handle.get());
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_support_format_all");
    }

    rv = archive_read_set_open_callback(handle.get(), openCallback);
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_set_open_callback");
    }
    rv = archive_read_set_close_callback(handle.get(), closeCallback);
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_set_close_callback");
    }
    rv = archive_read_set_read_callback(handle.get(), readCallback);
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_set_read_callback");
    }
    rv = archive_read_set_seek_callback(handle.get(), seekCallback);
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_set_seek_callback");
    }

    rv = archive_read_set_callback_data(handle.get(), context.get());
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_set_callback_data");
    }

    rv = archive_read_open1(handle.get());
    if (rv != ARCHIVE_OK) {
        throw ArchiveError(handle, "archive_read_open1");
    }

    return handle;
}


ArchiveHandle createDiskWriter () {
    ArchiveHandle handle(
        archive_write_disk_new(),
        [](ArchiveHandle::element_type * p) -> void {
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
        if (rv != ARCHIVE_OK) {
            throw ArchiveError(reader, "archive_read_data_block");
        }

        rv = archive_write_data_block(writer.get(), chunk, length, offset);
        if (rv != ARCHIVE_OK) {
            throw ArchiveError(writer, "archive_write_data_block");
        }
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
    using Buffer = Concurrency::streams::rawptr_buffer<uint8_t>;

    auto ctx = static_cast<Context *>(context);
    try {
        return ctx->readChunk(buffer);
    } catch (std::exception & e) {
        fprintf(stderr, "readCallback %s\n", e.what());
        return ARCHIVE_FATAL;
    }
}


la_int64_t seekCallback (struct archive * handle, void * context,
                         la_int64_t offset, int whence)
{
    auto ctx = static_cast<Context *>(context);
    auto ok = ctx->seek(offset, whence);
    return ok ? offset : ARCHIVE_FATAL;
}


web::uri makeBase (uint16_t port) {
    web::uri_builder builder;
    builder.set_scheme("http");
    builder.set_host("localhost");
    builder.set_port(port);
    return builder.to_uri();
}


web::uri makePath (const std::string & id) {
    std::ostringstream sout;
    sout << "/api/v1/nodes/" << id << "/stream";
    web::uri_builder builder;
    builder.set_path(sout.str());
    return builder.to_uri();
}


std::string resolvePath (const std::string & localPath, const std::string & id,
                         const std::string & entryName)
{
    std::ostringstream sout;
    for (auto c : entryName) {
        if (c < 0) {
            continue;
        }
        if (isalnum(c) || c == '.' || c == ' ' || c == '/') {
            sout << c;
        } else {
            sout << std::setfill('0') << std::setw(2) << std::hex
                 << static_cast<int>(c);
        }
    }
    boost::filesystem::path path = localPath;
    path /= id;
    path /= sout.str();
    return path.string();
}


Context::Context (uint16_t port, const std::string & id)
    : base(makeBase(port))
    , path(makePath(id))
    , response()
    , offset(0)
    , length(-1)
    , chunk()
{}


web::http::http_response &
Context::getResponse () {
    auto status = this->response.status_code();
    if (status == web::http::status_codes::OK ||
        status == web::http::status_codes::PartialContent) {
        return this->response;
    }

    web::http::http_request request;
    request.set_method(web::http::methods::GET);
    request.set_request_uri(this->path);
    if (this->length >= 0) {
        std::ostringstream sout;
        sout << "bytes=" << this->offset << "-" << (this->length - 1);
        request.headers().add("Range", sout.str());
    }

    web::http::client::http_client_config cfg;
    cfg.set_timeout(std::chrono::minutes(1));
    web::http::client::http_client client(this->base, cfg);
    this->response = client.request(request).get();
    status = this->response.status_code();
    if (status == web::http::status_codes::OK) {
        this->length = this->response.headers().content_length();
    } else if (status != web::http::status_codes::PartialContent) {
        throw HttpError(status, this->response.reason_phrase());
    }
    return this->response;
}


int64_t Context::readChunk (const void ** buffer) {
    using Buffer = Concurrency::streams::rawptr_buffer<uint8_t>;

    auto & response = this->getResponse();
    Buffer glue(&this->chunk[0], CHUNK_SIZE);
    auto length = response.body().read(glue, CHUNK_SIZE).get();
    *buffer = &this->chunk[0];
    this->offset += length;
    return length;
}


bool Context::seek (int64_t offset, int whence) {
    this->response = web::http::http_response();

    switch (whence) {
    case SEEK_SET:
        this->offset = offset;
        break;
    case SEEK_CUR:
        this->offset += offset;
        break;
    case SEEK_END:
        if (this->length < 0) {
            return false;
        }
        this->offset = this->length + offset;
        break;
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
