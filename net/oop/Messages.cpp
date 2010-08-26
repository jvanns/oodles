// oodles
#include "Messages.hpp"
#include "Serialiser.hpp"

// Boost.serialization
#include <boost/serialization/string.hpp>

// STL
using std::string;

namespace oodles {
namespace net {
namespace oop {
namespace msg {

void
RegisterCrawler_::serialize(Reconstructor &archive,
                            unsigned int /* version */)
{
    archive >> cores;
    archive >> name;
}

void
RegisterCrawler_::serialize(Deconstructor &archive,
                            unsigned int /* version */) const
{
    archive << cores;
    archive << name;
}

} // msg
} // oop
} // net
} // oodles

