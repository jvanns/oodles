#ifndef OODLES_NET_OOP_ACCESSOR_HPP // Interface
#define OODLES_NET_OOP_ACCESSOR_HPP

namespace oodles {
namespace net {
namespace oop {

struct Accessor; // Forward declaration for PERMIT_SERIALISER_ACCESS

} // oop
} // net
} // oodles

#define PERMIT_SERIALISER_ACCESS friend struct oodles::net::oop::Accessor;

#endif
