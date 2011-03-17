// oodles
#include "SessionHandler.hpp"

// libc
#include <assert.h> // For assert()

namespace oodles {
namespace net {

void
SessionHandler::set_endpoint(Endpoint::Connection e)
{
    assert(!endpoint);
    endpoint = e;
}

} // net
} // oodles
