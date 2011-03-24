#ifndef OODLES_EXCEPTIONS_HPP
#define OODLES_EXCEPTIONS_HPP

// oodles
#include "Exception.hpp"

namespace oodles {

// General oodles exceptions
struct OpenError : public NonFatalException
{
    OpenError(const std::string &from, int error, const char *format, ...);
};

struct ReadError : public NonFatalException
{
    ReadError(const std::string &from, int error, const char *format, ...);
};

struct WriteError : public NonFatalException
{
    WriteError(const std::string &from, int error, const char *format, ...);
};

// Net-specific exceptions
namespace net {

struct InvalidService : public FatalException
{
    InvalidService(const std::string &from, int error, const char *format, ...);
};

struct DNSFailure : public NonFatalException
{
    DNSFailure(const std::string &from, int error, const char *format, ...);
};

struct DialogError : public NonFatalException
{
    DialogError(const std::string &from, int error, const char *format, ...);
};

} // net

// URL-specific exceptions
namespace url {

struct ParseError : public NonFatalException
{
    ParseError(const std::string &from, int error, const char *format, ...);
};

} // url
} // oodles

#endif
