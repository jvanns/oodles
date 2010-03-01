#ifndef OODLES_EXCEPTION_HPP
#define OODLES_EXCEPTION_HPP

// STL
#include <string>
#include <exception>

#include <cstdarg> // For ...
#include <errno.h> // For errno

namespace oodles {

class Exception : public std::exception
{
    protected:
        int error; // The error number
        va_list list; // The argument list
        std::string message; // The resulting error message
        const std::string (*errno2str)(int error); // strerror() etc.
    protected:
        explicit Exception();
        virtual ~Exception() throw() = 0;

        void init(const std::string &from, int error, const char *format);
    public:
        int get_error() const {
            return error;
        }

        virtual const char* what() const throw();
};

/*
 * Use FatalException to terminate the application.
 */
struct FatalException : public Exception
{
    FatalException();
    ~FatalException() throw();
};

/*
 * Use NonFatalException for localised exceptions such as terminating
 * individual threads or recoverable failures (i.e. network etc.).
 */
struct NonFatalException : public Exception
{
    NonFatalException();
    virtual ~NonFatalException() throw();
};

} // oodles

#endif
