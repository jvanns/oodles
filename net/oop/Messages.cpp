// oodles
#include "Boost.hpp"
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

BeginCrawl_::BeginCrawl_() : pointer_owner(false)
{
}

BeginCrawl_::~BeginCrawl_()
{
}

void
BeginCrawl_::serialize(Reconstructor &archive,
                       unsigned int /* version */)
{
    archive >> urls;

    /*
     * The serialisation layer 'owns' (it created) the pointer(s)
     * when reconstructing the object on receipt of the raw data.
     */
    pointer_owner = true;
}

void
BeginCrawl_::serialize(Deconstructor &archive,
                       unsigned int /* version */) const
{
    archive << urls;

    /*
     * The serialisation layer does not own (did not create) the pointer(s)
     * when deconstructing the object to send as raw data. Therefore the
     * attribute 'pointer_owner' remains false (initialised by the constructor).
     */
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

