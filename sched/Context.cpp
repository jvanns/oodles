// oodles
#include "Context.hpp"
#include "utility/NodeIO.hpp"
#include "utility/Linker.hpp"

// Boost
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

// STL
using std::map;
using std::pair;
using std::string;
using std::ostream;

namespace {

/*
 * This namespace exists for this unit only so pull-in these types...
 */

// oodles   
using oodles::Proactor;
using oodles::io::DotMatrix;
using oodles::BreadCrumbTrail;
using oodles::sched::Scheduler;

// Boost
using boost::bind;
using boost::shared_ptr;
using boost::posix_time::seconds;
using boost::asio::deadline_timer;
using boost::enable_shared_from_this;

class ProactorTask : public enable_shared_from_this<ProactorTask>
{
    public:
        /* Member functions/methods */
        ProactorTask(Proactor &p, Scheduler &s, ostream *dot, int interval) :
            proactor(p),
            scheduler(s),
            dot_stream(dot),
            interval(interval),
            sleeper(p.io_service())
        {
            new_task();
        }
        
        void operator() ()
        {
#ifdef DEBUG_SCHED
            std::cerr << "Performing scheduling run...";
#endif
            
            BreadCrumbTrail trail;
            size_t then = time(NULL), assigned = scheduler.run(&trail), d = 0;
            
#ifdef DEBUG_SCHED
            std::cerr << assigned << " URLs assigned.\n";
#endif
            
            if (dot_stream) {
                DotMatrix dot(scheduler.url_tree());
                dot.set_trail(trail);
                *dot_stream << dot;
            }
               
            if ((d = time(NULL) - then) < interval) {
                sleeper.expires_from_now(seconds(interval - d));
                sleeper.async_wait(bind(&ProactorTask::new_task,
                                        shared_from_this()));
            } else {
                new_task();
            }
        }
        
        void new_task()
        {
            shared_ptr<ProactorTask> p(new ProactorTask(*this));
            p->proactor.io_service().post(bind(&ProactorTask::operator(), p));
        }
    private:
        /* Member variables/attributes */
        Proactor &proactor;
        Scheduler &scheduler;
        
        ostream *dot_stream;
        const int interval;
        deadline_timer sleeper;

        /* Member functions/methods */
        ProactorTask(const ProactorTask &t) :
            proactor(t.proactor),
            scheduler(t.scheduler),
            dot_stream(t.dot_stream),
            interval(t.interval),
            sleeper(t.proactor.io_service())
        {
        }
};

} // anonymous

namespace oodles {
namespace sched {

Context::Context() : server(proactor.io_service(), creator), scheduler(this)
{
}

void
Context::start_crawling(ostream *dot_stream, int interval)
{
    ProactorTask t(proactor, scheduler, dot_stream, interval);
    proactor.wait();
}

void
Context::start_server(const string &service)
{
    const Link link(&server, &scheduler);
    server.start(service);
}

void
Context::seed_scheduler(const std::string &url)
{
    scheduler.schedule_from_seed(url);
}

Crawler&
Context::create_crawler(const string &name, uint16_t cores)
{
    typedef map<string, Crawler>::value_type Value;
    typedef map<string, Crawler>::iterator Iterator;
    pair<Iterator, bool> x(crawlers.insert(Value(name, Crawler(name, cores))));

    return x.first->second;
}

} // sched
} // oodles
