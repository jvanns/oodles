#ifndef OODLES_URL_PARSER_HPP
#define OODLES_URL_PARSER_HPP

// STL
#include <string>
#include <vector>

namespace oodles {
namespace url {

struct Attributes
{
    bool ip; // Is the domain stored in fact an IP address?
    
    std::string port,
                page,
                scheme,
                username,
                password;
    std::vector<std::string> path, domain;
    
    Attributes() : ip(false) {}
};

struct Parser
{
    bool parse(const std::string &url, Attributes &attributes);
};

} // url
} // oodles

#endif

