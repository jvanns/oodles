// oodles
#include "Session.hpp"
#include "Context.hpp"
#include "Crawler.hpp"
#include "Scheduler.hpp"

// Boost.thread
#include <boost/thread/locks.hpp>

// STL
using std::map;
using std::string;
using std::vector;
using std::exception;

namespace oodles {
namespace sched {
namespace oop {

using net::DialogError;
using net::oop::END_CRAWL;
using net::oop::INVALID_ID;
using net::oop::BEGIN_CRAWL;
using net::oop::REGISTER_CRAWLER;

/* This array holds the valid subset of messages understood by this dialog */
const id_t Session::message_subset[] = {
    REGISTER_CRAWLER,
    BEGIN_CRAWL,
    END_CRAWL
};

// GarbageCollector
void
Session::GarbageCollector::receive(const event::Event::Ref e)
{
    const sched::Update &d = *e;
    const boost::lock_guard<boost::mutex> lock(mutex);
    map<Session::key_t, Message*>::iterator i = garbage.find(d.key);

    assert(i != garbage.end());

    delete i->second;
    garbage.erase(i);
}

void
Session::GarbageCollector::trash(Message *m, key_t k)
{
    if (k != 0) {
        const boost::lock_guard<boost::mutex> lock(mutex);
        garbage[k] = m;
        return;
    }
    
    delete m;
}

// Session
Session::Session()
{
    msg_context[Inbound] = msg_context[Outbound] = INVALID_ID;
}

void
Session::handle_message(Message *m)
{
    try {
        garbage.trash(m, continue_dialog(m));
    } catch (const exception &e) {
        delete m;
        throw;
    }
}

void
Session::begin_crawl(const vector<url::URL*> &urls)
{
    BeginCrawl *m = new BeginCrawl;
    vector<url::URL*>::const_iterator i = urls.begin(), j = urls.end();
    
    while (i != j) {
        m->urls.push_back(*i);
        ++i;
    }

    send(m);
}

void
Session::send(Message *m)
{
    msg_context[Outbound] = m->id();
    push_message(m);
}

inline
Context&
Session::context() const
{
    return *static_cast<Context*>(coupling->complement_of(*this));
}

inline
Scheduler&
Session::scheduler() const
{
    return context().get_scheduler();
}

Session::key_t
Session::continue_dialog(const Message *m)
throw (DialogError)
{
   /*
    * Returned key from continue_dialog handlers indicate whether or not to
    * delete and free-up the message immediately or rely on the event-driven
    * garbage collector to defer it until later once the data has been used
    * and released.
    */
    key_t k = 0;
    
    /*
     * Verify the context is valid based on this *incoming* message,
     * the previous outbound message and previous inbound message.
     */
    switch (m->id()) {
        /* Scheduler Inbound */
        case REGISTER_CRAWLER:
            if (msg_context[Inbound] != INVALID_ID)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid inbound context: P=#%d, C=#%d.",
                                   msg_context[Inbound], m->id());
            
            if (msg_context[Outbound] != INVALID_ID)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid outbound context: P=#%d, C=#%d.",
                                   msg_context[Outbound], m->id());

            k = continue_dialog(static_cast<const RegisterCrawler&>(*m));
            break;
        case END_CRAWL:
            if (msg_context[Inbound] != END_CRAWL &&
                msg_context[Inbound] != REGISTER_CRAWLER)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid inbound context: P=#%d, C=#%d.",
                                   msg_context[Inbound], m->id());
            
            if (msg_context[Outbound] != INVALID_ID &&
                msg_context[Outbound] != BEGIN_CRAWL)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid outbound context: P=#%d, C=#%d.",
                                   msg_context[Outbound], m->id());

            k = continue_dialog(static_cast<const EndCrawl&>(*m));
            break;
        default:
            throw DialogError("Session::continue_dialog",
                               0,
                               "Received unexpected OOP message #%d.",
                               m->id());
    }
    
    msg_context[Inbound] = m->id();

    return k;
}

Session::key_t
Session::continue_dialog(const RegisterCrawler &m)
{
    /*
     * Register, with the sched/Scheduler, a new sched/Crawler
     * based on this incoming data (name, cores etc.)
     */
    Crawler &c = context().create_crawler(m.name, m.cores);
    scheduler().register_crawler(c);
    c.set_session(this);
    
#ifdef DEBUG_SCHED
    std::cerr << "Registered crawler '" << m.name
              << "' with " << m.cores << " cores.\n";
#endif

    return 0;
}

Session::key_t
Session::continue_dialog(const EndCrawl &m)
{
    /*
     * Update the URL-tree in the scheduler with the data
     * contained in the received message, m, as sent
     * by the Crawler. We defer the update, however.
     */
    key_t k = reinterpret_cast<key_t>(&m);
    const sched::Deferable update = {k, m.new_urls, m.scheduled_urls};

    scheduler().defer_update(update, garbage);

    return k; // We delete later, upon the garbage collector receive()
}

} // oop
} // sched
} // oodles
