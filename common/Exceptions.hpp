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

// URL-specific exceptions
namespace url {

struct ParseError : public NonFatalException
{
    ParseError(const std::string &from, int error, const char *format, ...);
};

} // url
} // oodles

#endif
