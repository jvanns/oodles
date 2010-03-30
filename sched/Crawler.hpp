#ifndef OODLES_SCHED_CRAWLER_HPP
#define OODLES_SCHED_CRAWLER_HPP

// STL
#include <vector>
#include <functional>

namespace oodles {

namespace url {
    class URL; // Forward declaration for Crawler
} // url

namespace net {
    class Endpoint; // Forward declaration for Crawler
} // net

namespace sched {

struct RankCrawler; // Forward declaration for Crawler

class Crawler
{
    public:
        /* Member variables/attributes */
        Crawler();
    private:
        /* Member variables/attributes */
        net::Endpoint *endpoint; // Network session associated with this Crawler
        std::vector<url::URL*> work_unit; // N units of work (URLs to crawl)

        /* Friend class declarations */
        friend struct RankCrawler;
};

struct RankCrawler : std::binary_function<Crawler, Crawler, bool>
{
    bool operator() (const Crawler *lhs, const Crawler *rhs) const;
};

} // sched
} // oodles

#endif
