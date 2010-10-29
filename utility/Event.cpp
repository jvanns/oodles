// oodles
#include "Event.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// STL
using std::set;

namespace oodles {
namespace event {

Event::Event()
{
}

Event::~Event()
{
    set<Subscriber*>::iterator i = subscribers.begin(), j = subscribers.end();

    while (i != j) {
        Subscriber &s = *(*i);
        remove_subscriber(s);
        ++i;
    }
}

void
Event::publish(const Publisher &p) const
{
    p.broadcast(*this);
}

bool
Event::add_subscriber(Subscriber &s)
{
    return s.subscribe_to(*this);
}

bool
Event::remove_subscriber(Subscriber &s)
{
    return s.unsubscribe_from(*this);
}

} // event
} // oodles

