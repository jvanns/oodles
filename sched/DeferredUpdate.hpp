#ifndef OODLESS_SCHED_DEFERRED_UPDATE_HPP
#define OODLESS_SCHED_DEFERRED_UPDATE_HPP

// oodles
#include "utility/Event.hpp"
#include "utility/Publisher.hpp"
#include "net/oop/Messages.hpp"

// STL
#include <list>

namespace oodles {
namespace sched {

class Scheduler; // Forward declaration for DeferredUpdate

/*
 * A Deferable is in fact more specifically a Deferable schedule update.
 * It holds a key by which the memory holding the data referenced by
 * new_urls and scheduled_urls can be freed later using the events system.
 */
struct Deferable
{
    typedef ptrdiff_t key_t;
    typedef net::oop::EndCrawl::NewURLs NewURLs;
    typedef net::oop::EndCrawl::ScheduledURLs ScheduledURLs;

    const key_t key;
    const NewURLs &new_urls;
    const ScheduledURLs &scheduled_urls;
};

/*
 * Update is used internally and is slightly more bloated than Deferable as
 * it must inherit from Event. Deferable must be kept light-weight as it is
 * copied (though only once).
 */
struct Update : public Deferable, public event::Event
{
    Update(const Deferable &d) : Deferable(d) {}
    event::Event* clone() const { return new Update(*this); }
};

class DeferredUpdate
{
    public:
        /* Member functions/methods */
        DeferredUpdate(Dispatcher &d);
        
        uint32_t update(Scheduler &s);
        void defer(const Deferable &d, event::Subscriber &s);
    private:
        /* Member variables/attributes */
        std::list<Update> updates;
        event::Publisher publisher;
};

} // sched
} // oodles

#endif

