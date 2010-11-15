// oodles
#include "Event.hpp"
#include "Proactor.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost
using boost::bind;

// STL
using std::set;

namespace oodles {
namespace event {

Publisher::Publisher(Proactor *p) : proactor(p)
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

        if (proactor)
            proactor->io_service().post(bind(&Subscriber::receive, s, ref));
        else
            s->receive(ref); // Will block

        ++i;
    }
}

} // event
} // oodles

