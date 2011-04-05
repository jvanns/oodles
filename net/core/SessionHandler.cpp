// oodles
#include "SessionHandler.hpp"
#include "ProtocolHandler.hpp"

// libc
#include <assert.h> // For assert()

// STL
using std::ostream;

namespace oodles {
namespace net {

void
SessionHandler::set_endpoint(Endpoint::Connection e)
{
    assert(!endpoint);
    endpoint = e;
}

void
SessionHandler::print_metrics(ostream *s) const
{
    if (!endpoint)
        return;
    
    endpoint->print_metrics(s);
    endpoint->get_protocol()->print_metrics(s);
}

} // net
} // oodles
