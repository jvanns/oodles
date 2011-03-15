// oodles
#include "Event.hpp"
#include "Publisher.hpp"
#include "Dispatcher.hpp"
#include "Subscriber.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost
using boost::bind;

// STL
using std::set;

namespace oodles {
namespace event {

Publisher::Publisher(Dispatcher *d) : dispatcher(d)
{
}

void
Publisher::broadcast(const Event &e) const
{
    const Event::Ref ref(e.clone());
    set<Subscriber*>::iterator i = e.subscribers.begin(),
                               j = e.subscribers.end();

    while (i != j) {
        Subscriber *s = *i;

        if (dispatcher)
            dispatcher->io_service().post(bind(&Subscriber::receive, s, ref));
        else
            s->receive(ref); // Will block

        ++i;
    }
}

} // event
} // oodles

