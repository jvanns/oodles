// oodles
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
}

Publisher::Publisher(Event &e) : object(&e)
{
}

Publisher::~Publisher()
{
    set<Subscriber*>::const_iterator i = subscribers.begin(),
                                     j = subscribers.end();

    while (i != j) {
        (*i)->unsubscribe_from(*this);
        ++i;
    }
}

void
Publisher::broadcast() const
{
    set<Subscriber*>::const_iterator i = subscribers.begin(),
                                     j = subscribers.end();

    while (i != j) {
        (*i)->receive(*this);
        ++i;
    }
}

bool
Publisher::add_subscriber(Subscriber &s)
{
    const bool rc = s.subscribe_to(*this);

    if (rc)
        subscribers.insert(&s);

    return rc;
}

bool
Publisher::remove_subscriber(Subscriber &s)
{
    const bool rc = s.subscribed_to(*this);

    if (rc) {
        subscribers.erase(&s);
        s.clear_publisher();
    }

    return rc;
}

} // event
} // oodles

