// oodles
#include "Publisher.hpp"
#include "Subscriber.hpp"

namespace oodles {
namespace event {

Subscriber::Subscriber() : publisher(NULL)
{
}

Subscriber::~Subscriber()
{
}

bool
Subscriber::subscribe_to(Publisher &p)
{
    if (!subscribed()) {
        publisher = &p;
        return true;
    }

    if (subscribed_to(p))
        return true; // We already are!

    return false; // We're already subscribed to something else!
}

bool
Subscriber::unsubscribe_from(Publisher &p)
{
    return p.remove_subscriber(*this);
}

} // event
} // oodles

