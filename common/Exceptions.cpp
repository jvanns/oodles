// oodles
#include "Exceptions.hpp"

// STL
using std::string;

namespace oodles {
   
ReadError::ReadError(const string &from, int error, const char *format, ...)
{
    va_start(list, format);
    init(from, error, format); // va_end happens in here
}

} // oodles

