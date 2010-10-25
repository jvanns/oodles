// oodles
#include "Scheduler.hpp"
#include "DeferredUpdate.hpp"
#include "utility/Subscriber.hpp"

// STL
using std::list;

namespace oodles {
namespace sched {

DeferredUpdate::DeferredUpdate(Proactor &p) : publisher(&p)
{
}

void
DeferredUpdate::defer(const Deferable &d, event::Subscriber &s)
{
    updates.push_back(d);
    updates.back().add_subscriber(s);
}

uint32_t
DeferredUpdate::update(Scheduler &s)
{
    uint32_t x = 0;
    url::URL::hash_t id = 0;
    const time_t now = time(NULL);
    Deferable::NewURLs::const_iterator i, j;
    Deferable::ScheduledURLs::const_iterator k, l;
    list<Update>::const_iterator m = updates.begin(), n = updates.end();

    for ( ; m != n ; ++m) {
        for (i = m->new_urls.begin(), j = m->new_urls.end() ;
             i != j ; 
             ++i)
        {
            id = s.schedule_from_crawl(i->first);

            if (i->second)
                s.update_node(id, now);
        }
        
        for (k = m->scheduled_urls.begin(), l = m->scheduled_urls.end() ; 
             k != l ;
             ++k)
        {
            ++x;
            id = k->first;
            s.update_node(id, now);
        }

        /*
         * Inform the subscriber(s), the garbage collector(s), that we are
         * done with the crawl information it holds and it can now be removed
         * at it's leisure.
         */
        m->publish(publisher);
    }

    updates.clear();

    return x;
}

} // sched
} // oodles

