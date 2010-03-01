// oodles
#include "Exception.hpp"

#include <stdio.h> // For vsnprintf

// STL
using std::string;

static
const string
default_errno2str(const int error)
{
    string s;

    if (error) { // Anything non-zero is an error
        const char *message = strerror(error);

        if (message)
            s = message;
    }
    
    return s;
}

namespace oodles {

Exception::Exception () : error(0),
                          message("Exception thrown from "),
                          errno2str(default_errno2str)
{
}

Exception::~Exception() throw()
{
}

void
Exception::init(const string &from, int e, const char *format)
{
    static const uint16_t buffer_size = 2048;
    char buffer[buffer_size];

    memset(buffer, '\0', buffer_size);
    vsnprintf(buffer, buffer_size, format, list);
    va_end(list);

    message += (from + ":\n** " + buffer);

    if ((error = e) && errno2str) {
        const string s(errno2str(e));

        if (!s.empty())
            message += "\n** External 'strerror' function suggests: " + s;
    }
}

const char* Exception::what() const throw()
{
    return message.c_str ();
}

FatalException::FatalException()
{
}

FatalException::~FatalException() throw()
{
}

NonFatalException::NonFatalException()
{
}

NonFatalException::~NonFatalException() throw()
{
}

} // oodles

