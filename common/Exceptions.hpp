#ifndef OODLES_EXCEPTIONS_HPP
#define OODLES_EXCEPTIONS_HPP

// oodles
#include "Exception.hpp"

namespace oodles {

struct ReadError : public NonFatalException
{
    ReadError(const std::string &from, int error, const char *format, ...);
};

} // oodles

#endif
