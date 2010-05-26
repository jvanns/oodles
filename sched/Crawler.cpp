// oodles
#include "Crawler.hpp"
#include "url/URL.hpp"

// STL
using std::string;

namespace  oodles {
namespace sched {

Crawler::Crawler(const string &name) : name(name), endpoint(NULL)
{
}

uint16_t
Crawler::add_url(const url::URL &url)
{
    work_unit.push_back(&url);

#ifdef DEBUG_SCHED
    std::cerr << '[' << name << "]: "
              << *(work_unit.back())
              << " assigned\n";
#endif

    return unit_size();
}

bool
RankCrawler::operator() (const Crawler *lhs, const Crawler *rhs) const
{
    /*
     * First, be sure to check that the Crawlers have a valid Endpoint!
     */
    if (lhs->online() && rhs->online()) {
        if (lhs->unit_size() == rhs->unit_size())
            return false; // Maintain a stable queue

        /*
         * Always prefer Crawlers with less work at the front of the queue
         */
        return lhs->unit_size() > rhs->unit_size();
    }

    /*
     * If neither are online, then...
     */
    if (!lhs->online() && !rhs->online())
        return false; // Again, maintain a stable queue

    if (!lhs->online())
        return true;

    return false;
}

} // sched
} // oodles
