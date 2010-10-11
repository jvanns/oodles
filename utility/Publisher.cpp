// oodles
#include "Proactor.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost.asio
using boost::bind;

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

Publisher::Publisher(Event &e, Proactor *p) : object(&e), proactor(p)
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

size_t
Publisher::broadcast() const
{
    size_t n = 0;
    const Publisher &p = *this;
    set<Subscriber*>::const_iterator i = subscribers.begin(),
                                     j = subscribers.end();

    while (i != j) {
        Subscriber *s = *i;

        if (proactor) {
            proactor->io_service().post(bind(&Subscriber::receive, s, p));
            ++n;
        } else
            s->receive(p); // Will block

        ++i;
    }

    return n;
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

