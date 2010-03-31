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
        uint16_t add_url(const url::URL *url);

        static uint16_t max_unit_size() { return 32; }
        bool online() const { return endpoint != NULL; }
        uint16_t unit_size() const { return work_unit.size(); }
    private:
        /* Member variables/attributes */
        net::Endpoint *endpoint; // Network session associated with this Crawler
        std::vector<const url::URL*> work_unit; // Units of work (URLs to crawl)
};

struct RankCrawler : std::binary_function<Crawler, Crawler, bool>
{
    bool operator() (const Crawler *lhs, const Crawler *rhs) const;
};

} // sched
} // oodles

#endif
