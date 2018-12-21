#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


#include <exception>
#include <string>

#include <cpprest/http_client.h>

#include "types.hpp"


class ArchiveError : public std::exception {
public:
    ArchiveError (ArchiveHandle handle, const std::string & name) noexcept;

    const char * what () const noexcept override;

private:
    std::string msg;
};


class EntryError : public std::exception {
public:
    EntryError (const std::string & name, const std::string & detail) noexcept;

    const char * what () const noexcept override;

private:
    std::string msg;
};


class HttpError : public std::exception {
public:
    HttpError (web::http::status_code status,
               const web::http::reason_phrase & reason) noexcept;

    const char * what () const noexcept override;

private:
    std::string msg;
};


#endif
