// oodles
#include "Message.hpp"
#include "Factory.hpp"

namespace oodles {
namespace net {
namespace oop {

Factory::Factory() : index(MAX_MESSAGE_ID)
{
}

Factory::~Factory()
{
}


Factory&
Factory::instance()
{
    static Factory singleton;
    return singleton;
}

bool
Factory::enroll(const Message *m)
{
    if (!index.set_bits(m->id()))
        return false;

    size_t i = index.index_of(m->id()), s = registry.size() + 1;

    if (i < s)
        registry.resize(s, NULL);

    registry[i] = m;

    return true;
}

const Message*
Factory::operator[] (size_t subscript) const
{
    if (subscript > registry.size())
        return NULL;

    return registry[subscript];
}

} // oop
} // net
} // oodles
