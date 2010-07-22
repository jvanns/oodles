// oodles
#include "Exceptions.hpp"

// STL
using std::string;

namespace oodles {

// General oodles exceptions
OpenError::OpenError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

ReadError::ReadError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

WriteError::WriteError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

// Net-specific exceptions
namespace net {

InvalidService::InvalidService(const string &from,
                               int error,
                               const char *format,
                               ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

} // net

// URL-specific exceptions
namespace url {

ParseError::ParseError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

} // url
} // oodles

