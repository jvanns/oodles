// oodles
#include "Event.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// STL
using std::set;

namespace oodles {
namespace event {

// Public methods

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

// Protected methods

Event::Event(const Event &e)
{
    /*
     * Do not copy the subscribers. Only Publisher::broadcast() should make
     * a copy/clone of us. The source Event, 'e', remains the authority as
     * far as subscriptions are concerned. When it ('e') goes out of scope
     * or is deleted then the subcribers are removed as above in ~Event().
     */
}

Event&
Event::operator= (const Event &e)
{
    /*
     * Do not copy the subscribers. Only Publisher::broadcast() should make
     * a copy/clone of us. The source Event, 'e', remains the authority as
     * far as subscriptions are concerned. When it ('e') goes out of scope
     * or is deleted then the subcribers are removed as above in ~Event().
     */

    return *this;
}

} // event
} // oodles

