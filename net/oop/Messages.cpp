// oodles
#include "Messages.hpp"

// Boost.serialization
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

// STL
using std::string;

namespace {

bool init()
{
    oodles::net::oop::RegisterCrawler::init();
    oodles::net::oop::BeginCrawl::init();
    oodles::net::oop::EndCrawl::init();
    
    return true;
}

static const bool factory_initialised = init();

} // anonymous

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

void
BeginCrawl_::serialize(Reconstructor &archive,
                       unsigned int /* version */)
{
    archive >> urls;
}

void
BeginCrawl_::serialize(Deconstructor &archive,
                       unsigned int /* version */) const
{
    archive << urls;
}

void
EndCrawl_::serialize(Reconstructor &archive,
                     unsigned int /* version */)
{
    archive >> new_urls;
    archive >> scheduled_urls;
}

void
EndCrawl_::serialize(Deconstructor &archive,
                     unsigned int /* version */) const
{
    archive << new_urls;
    archive << scheduled_urls;
}

} // msg
} // oop
} // net
} // oodles

