#ifndef OODLES_SCHED_CRAWLER_HPP
#define OODLES_SCHED_CRAWLER_HPP

// STL
#include <vector>
#include <string>
#include <functional>

#include <stdint.h> // For uint16_t

namespace oodles {

namespace url {
    class URL; // Forward declaration for Crawler
} // url

namespace net {
    class Endpoint; // Forward declaration for Crawler
} // net

namespace sched {

class Crawler
{
    public:
        /* Dependent typedefs */
        typedef uint16_t unit_t;

        /* Member variables/attributes */
        Crawler(const std::string &name);
        unit_t add_url(const url::URL &url);

        static unit_t max_unit_size() { return 32; }

        const std::string& id() const { return name; }
        bool online() const { return endpoint != NULL; }
        unit_t unit_size() const { return work_unit.size(); }
    private:
        /* Member variables/attributes */
        std::string name; // Identifier for this Crawler (i.e. hostname)
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
