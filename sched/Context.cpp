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

// libc
#include <sys/time.h>
#include <sys/resource.h>

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
using oodles::Dispatcher;
using oodles::io::DotMatrix;
using oodles::BreadCrumbTrail;
using oodles::sched::Scheduler;

// Boost
using boost::bind;
using boost::shared_ptr;
using boost::posix_time::seconds;
using boost::asio::deadline_timer;
using boost::enable_shared_from_this;

class DispatcherTask : public enable_shared_from_this<DispatcherTask>
{
    public:
        /* Member functions/methods */
        DispatcherTask(Dispatcher &d, Scheduler &s, ostream *dot, int interval):
            scheduler(s),
            dispatcher(d),
            dot_stream(dot),
            interval(interval),
            sleeper(d.io_service())
        {
            new_task();
        }
        
        void operator() ()
        {
            struct rusage x, y;
            BreadCrumbTrail trail;
            size_t then = time(NULL), assigned = 0, d = 0;
            
#ifdef DEBUG_SCHED
            std::cerr << "Performing schedule update...";
#endif
            getrusage(RUSAGE_SELF, &x);
            assigned = scheduler.update_schedule();
            getrusage(RUSAGE_SELF, &y);

#ifdef DEBUG_SCHED
            std::cerr << assigned << " URLs updated.\n";
            std::cerr << "\tRun took " << y.ru_utime.tv_sec - x.ru_utime.tv_sec
                      << '.' << abs(y.ru_utime.tv_usec - x.ru_utime.tv_usec)
                      << "s/u, " << y.ru_stime.tv_sec - x.ru_stime.tv_sec
                      << '.' << abs(y.ru_stime.tv_usec - x.ru_stime.tv_usec)
                      << "s/s\n";
#endif
            
#ifdef DEBUG_SCHED
            std::cerr << "Performing scheduling run...";
#endif

            getrusage(RUSAGE_SELF, &x);
            assigned = scheduler.run(&trail);
            getrusage(RUSAGE_SELF, &y);
            
#ifdef DEBUG_SCHED
            std::cerr << assigned << " URLs assigned.\n";
            std::cerr << "\tRun took " << y.ru_utime.tv_sec - x.ru_utime.tv_sec
                      << '.' << abs(y.ru_utime.tv_usec - x.ru_utime.tv_usec)
                      << "s/u, " << y.ru_stime.tv_sec - x.ru_stime.tv_sec
                      << '.' << abs(y.ru_stime.tv_usec - x.ru_stime.tv_usec)
                      << "s/s\n";
#endif

            if (dot_stream) {
#ifdef DEBUG_SCHED
                std::cerr << "Writing DOT graph of schedule run...";
#endif
                DotMatrix dot(scheduler.url_tree());
                dot.set_trail(trail);
                *dot_stream << dot;
#ifdef DEBUG_SCHED
                std::cerr << "done.\n";
#endif
            }
            
            if ((d = time(NULL) - then) < interval) {
                sleeper.expires_from_now(seconds(interval - d));
                sleeper.async_wait(bind(&DispatcherTask::new_task,
                                        shared_from_this()));
            } else {
                new_task();
            }
        }
        
        void new_task()
        {
            shared_ptr<DispatcherTask> task(new DispatcherTask(*this));
            task->dispatcher.io_service().post(bind(&DispatcherTask::operator(),
                                                    task));
        }
    private:
        /* Member variables/attributes */
        Scheduler &scheduler;
        Dispatcher &dispatcher;
        
        ostream *dot_stream;
        const int interval;
        deadline_timer sleeper;

        /* Member functions/methods */
        DispatcherTask(const DispatcherTask &t) :
            scheduler(t.scheduler),
            dispatcher(t.dispatcher),
            dot_stream(t.dot_stream),
            interval(t.interval),
            sleeper(t.dispatcher.io_service())
        {
        }
};

} // anonymous

namespace oodles {
namespace sched {

Context::Context() : server(dispatcher.io_service(), creator), scheduler(this)
{
}

void
Context::stop_crawling()
{
    dispatcher.stop();
}

void
Context::start_crawling(ostream *dot_stream, int interval)
{
    DispatcherTask t(dispatcher, scheduler, dot_stream, interval);
    dispatcher.wait();
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
