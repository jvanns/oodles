// oodles
#include "Exceptions.hpp"

// STL
using std::string;

namespace oodles {

// General oodles exceptions
ReadError::ReadError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

// URL-specific exceptions
namespace url {

ParseError::ParseError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

} // url
} // oodles

