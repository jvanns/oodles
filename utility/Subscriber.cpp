// oodles
#include "Event.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// STL
using std::map;
using std::pair;

namespace oodles {
namespace event {

Subscriber::Subscriber()
{
}

Subscriber::~Subscriber()
{
    map<Event*, Locator>::const_iterator i = events.begin(), j = events.end();

    while (i != j) {
        i->first->subscribers.erase(i->second);
        ++i;
    }
}

bool
Subscriber::subscribe_to(Event &e)
{
    static const Locator l; // Default placeholder
    const pair<Event*, Locator> item(&e, l);
    pair<map<Event*, Locator>::iterator, bool> x(events.insert(item));

    if (x.second)
        x.first->second = e.subscribers.insert(this).first;

    return x.second;
}

bool
Subscriber::unsubscribe_from(Event &e)
{
    map<Event*, Locator>::iterator i = events.find(&e);
    bool found = i != events.end();

    if (found)
        e.subscribers.erase(i->second);

    return found;
}

} // event
} // oodles

