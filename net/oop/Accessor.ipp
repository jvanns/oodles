#ifndef OODLES_NET_OOP_ACCESSOR_IPP // Implementation
#define OODLES_NET_OOP_ACCESSOR_IPP

// oodles
#include "url/URL.hpp"
#include "Accessor.hpp"

// Boost.serialization
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace oodles {
namespace net {
namespace oop {

struct Accessor
{
    template<class Archive>
    static
    void load(Archive &archive, url::URL &url, unsigned int version)
    {
        archive >> url.id.page;
        archive >> url.id.path;
        archive >> url.id.domain;

        archive >> url.attributes.ip;

        archive >> url.attributes.port;
        archive >> url.attributes.page;
        archive >> url.attributes.scheme;
        archive >> url.attributes.username;
        archive >> url.attributes.password;

        archive >> url.attributes.path;
        archive >> url.attributes.domain;
    }

    template<class Archive>
    static
    void save(Archive &archive, const url::URL &url, unsigned int version)
    {
        archive << url.id.page;
        archive << url.id.path;
        archive << url.id.domain;

        archive << url.attributes.ip;

        archive << url.attributes.port;
        archive << url.attributes.page;
        archive << url.attributes.scheme;
        archive << url.attributes.username;
        archive << url.attributes.password;

        archive << url.attributes.path;
        archive << url.attributes.domain;
    }
};

} // oop
} // net
} // oodles

#endif
