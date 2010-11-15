// OOP (oodles protocol)
#include "Messages.hpp"
#include "SchedulerCrawler.hpp"

// oodles scheduler
#include "sched/Context.hpp"
#include "sched/Crawler.hpp"
#include "sched/Scheduler.hpp"

// Boost.thread
#include <boost/thread/locks.hpp>

// STL
using std::map;
using std::list;
using std::string;
using std::vector;
using std::make_pair;
using std::exception;

namespace oodles {
namespace net {
namespace oop {
namespace dialect {

/* This array holds the valid subset of messages understood by this dialect */
const id_t SchedulerCrawler::message_subset[] = {
    REGISTER_CRAWLER,
    BEGIN_CRAWL,
    END_CRAWL
};

// GarbageCollector
void
SchedulerCrawler::GarbageCollector::receive(const event::Event::Ref e)
{
    const sched::Update &d = *e;
    const boost::lock_guard<boost::mutex> lock(mutex);
    map<SchedulerCrawler::key_t, Message*>::iterator i = garbage.find(d.key);

    assert(i != garbage.end());

    delete i->second;
    garbage.erase(i);
}

void
SchedulerCrawler::GarbageCollector::trash(Message *m, key_t k)
{
    if (k != 0) {
        const boost::lock_guard<boost::mutex> lock(mutex);
        garbage[k] = m;
        return;
    }
    
    delete m;
}

// SchedulerCrawler
SchedulerCrawler::SchedulerCrawler() : initiator(false), crawler(NULL)
{
    context[Inbound] = context[Outbound] = INVALID_ID;
}

void
SchedulerCrawler::translate()
{
    Message *m = protocol().pop_message();
    
    try {
        while (m) {
            garbage.trash(m, continue_dialog(m));
            m = protocol().pop_message();
        }
    } catch (const exception &e) {
        delete m;
        throw;
    }
}

void
SchedulerCrawler::register_crawler(const string &name, uint16_t cores)
{
    RegisterCrawler *m = new RegisterCrawler;
    m->cores = cores;
    m->name = name;

    initiator = true;
    
    send(m); // Non-blocking
}

void
SchedulerCrawler::begin_crawl(const vector<url::URL*> &urls)
{
    BeginCrawl *m = new BeginCrawl;
    vector<url::URL*>::const_iterator i = urls.begin(), j = urls.end();
    
    while (i != j) {
        url::URL &u = *(*i);
        m->urls[u.domain_id()].push_back(BeginCrawl::URL(u.page_id(),
                                                         u.to_string()));
        ++i;
    }

    send(m);
}

void
SchedulerCrawler::send(Message *m)
{
    context[Outbound] = m->id();
    protocol().push_message(m);
}

Protocol&
SchedulerCrawler::protocol() const
{
    assert(handler != NULL);
    return *handler;
}

sched::Scheduler&
SchedulerCrawler::scheduler() const
{
    assert(coupling != NULL);

    Linker *l = coupling->complement_of(*this);

    assert(l != NULL);
    
    return static_cast<sched::Scheduler&>(*l);
}

SchedulerCrawler::key_t
SchedulerCrawler::continue_dialog(const Message *m)
throw (DialectError)
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
        /* Crawler Inbound */
        case BEGIN_CRAWL:
            if (!initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
            
            if (context[Inbound] != INVALID_ID &&
                context[Inbound] != BEGIN_CRAWL)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != END_CRAWL &&
                context[Outbound] != REGISTER_CRAWLER)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            k = continue_dialog(static_cast<const BeginCrawl&>(*m));
            break;
        /* Scheduler Inbound */
        case REGISTER_CRAWLER:
           if (initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
            
            if (context[Inbound] != INVALID_ID)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != INVALID_ID)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());

            k = continue_dialog(static_cast<const RegisterCrawler&>(*m));
            break;
        case END_CRAWL:
           if (initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
            
            if (context[Inbound] != END_CRAWL &&
                context[Inbound] != REGISTER_CRAWLER)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != INVALID_ID &&
                context[Outbound] != BEGIN_CRAWL)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());

            k = continue_dialog(static_cast<const EndCrawl&>(*m));
            break;
        default:
            throw DialectError("SchedulerCrawler::continue_dialog",
                               0,
                               "Received unexpected OOP message #%d.",
                               m->id());
    }
    
    context[Inbound] = m->id();

    return k;
}

SchedulerCrawler::key_t
SchedulerCrawler::continue_dialog(const RegisterCrawler &m)
{
    /*
     * Register, with the sched/Scheduler, a new sched/Crawler
     * based on this incoming data (name, cores etc.)
     */
    sched::Context *context = scheduler().context();
    
    assert(context != NULL);
    
    crawler = &(context->create_crawler(m.name, m.cores));
    crawler->set_endpoint(handler->get_endpoint());
    scheduler().register_crawler(*crawler);
    
#ifdef DEBUG_SCHED
    std::cerr << "Registered crawler '" << m.name
              << "' with " << m.cores << " cores.\n";
#endif

    return 0;
}

SchedulerCrawler::key_t
SchedulerCrawler::continue_dialog(const BeginCrawl &m)
{
    /*
     * Instruct the endpoint-owning Crawler to begin crawling
     * all URLs given in m as scheduled by the Scheduler that
     * sent this message.
     */
    EndCrawl *e = new EndCrawl;
    BeginCrawl::URLs::const_iterator i, j;

    for (i = m.urls.begin(), j = m.urls.end() ; i != j ; ++i) {
#ifdef DEBUG_CRAWL
        std::cerr << "Will crawl " << i->second.size() 
                  << " URLs from the domain ID of " << i->first << std::endl;
#endif

        list<BeginCrawl::URL>::const_iterator k, l;
        for (k = i->second.begin(), l = i->second.end() ; k != l ; ++k)
            e->scheduled_urls.push_back(make_pair(k->first, true));
    }

    send(e);

    return 0;
}

SchedulerCrawler::key_t
SchedulerCrawler::continue_dialog(const EndCrawl &m)
{
    /*
     * Update the URL-tree in the scheduler with the data
     * contained in the received message, m, as sent
     * by the Crawler. We defer the update, however.
     */
    assert(crawler != NULL);

    key_t k = reinterpret_cast<key_t>(&m);
    const sched::Deferable update = {k, m.new_urls, m.scheduled_urls};

    scheduler().defer_update(update, garbage);

    return k; // We delete later, upon the garbage collector receive()
}

} // dialect
} // oop
} // net
} // oodles
