// oodles
#include "ResponseCodes.hpp"
#include "common/Exceptions.hpp"

// libc
#include <assert.h> // For assert()

// STL
using std::string;
using std::vector;

namespace oodles {
namespace net {
namespace http {

ResponseCodes&
ResponseCodes::instance()
{
    static ResponseCodes singleton;
    return singleton;
}
   
const string&
ResponseCodes::operator[] (size_t code) const
{
    int32_t i = index.index_of(code);
    
    if (i == -1 || i > messages.size())
        throw RangeError("ResponseCodes::operator[]",
                         0,
                         "Message for code %d not found.",
                         i);

    return messages[i];
}

ResponseCodes::ResponseCodes() : index(505)
{
    /* Informational 1xx */
    add_message(100, "Continue");
    add_message(101, "Switching Protocols");
    
    /* Successful 2xx */
    add_message(200, "OK");
    add_message(201, "Created");
    add_message(202, "Accepted");
    add_message(203, "Non-Authoritative Information");
    add_message(204, "No Content");
    add_message(205, "Reset Content");
    add_message(206, "Partial Content");

    /* Redirection 3xx */
    add_message(300, "Multiple Choices");
    add_message(301, "Moved Permanently");
    add_message(302, "Found");
    add_message(303, "See Other");
    add_message(304, "Not Modified");
    add_message(305, "Use Proxy");
    add_message(306, "(Unused)");
    add_message(307, "Temporary Redirect");

    /* Client Error 4xx */
    add_message(400, "Bad Request");
    add_message(401, "Unauthorized");
    add_message(402, "Payment Required");
    add_message(403, "Forbidden");
    add_message(404, "Not Found");
    add_message(405, "Method Not Allowed");
    add_message(406, "Not Acceptable");
    add_message(407, "Proxy Authentication Required");
    add_message(408, "Request Timeout");
    add_message(409, "Conflict");
    add_message(410, "Gone");
    add_message(411, "Length Required");
    add_message(412, "Precondition Failed");
    add_message(413, "Request Entity Too Large");
    add_message(414, "Request URI Too Long");
    add_message(415, "Unsupported Media Type");
    add_message(416, "Requested Range Not Satisfiable");
    add_message(417, "Expectation Failed");

    /* Server Error 5xx */
    add_message(500, "Internal Server Error");
    add_message(501, "Not Implemented");
    add_message(502, "Bad Gateway");
    add_message(503, "Service Unavailable");
    add_message(504, "Gateway Timeout");
    add_message(505, "HTTP Version Not Supported");
}

ResponseCodes::~ResponseCodes()
{
}

void
ResponseCodes::add_message(uint16_t code, const string& message)
{
    bool ok = index.set_bits(code);
    size_t i = index.index_of(code), s = messages.size() + 1;

    assert(ok);

    if (i < s)
        messages.resize(s, "");

    messages[i] = message;
}

} // http
} // net
} // oodles
