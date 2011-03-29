#ifndef OODLES_SCHED_CRAWLER_HPP
#define OODLES_SCHED_CRAWLER_HPP

// oodles
#include "Session.hpp"

// STL
#include <vector>
#include <string>
#include <functional>

#include <stdint.h> // For uint16_t

namespace oodles {

namespace url {
    class URL; // Forward declaration for Crawler
} // url

namespace sched {

class Crawler
{
    public:
        /* Dependent typedefs */
        typedef uint16_t unit_t;

        /* Member functions/methods */
        Crawler(const std::string &name, uint16_t cores = 1);

        void begin_crawl();
        unit_t add_url(url::URL &url);
        unit_t remove_url(url::URL &url);
        void set_session(oop::Session *s);

        bool online() const { return !offline(); }
        const std::string& id() const { return name; }

        unit_t max_unit_size() const { return 32 * cores; }
        unit_t assigned() const { return work_unit.size(); }
        bool full() const { return assigned() == max_unit_size(); }
    private:
        /* Member functions/methods */
        bool offline() const { return !session || !session->online(); }

        /* Member variables/attributes */
        oop::Session *session; // Network session for this Crawler
        
        const uint16_t cores;
        const std::string name; // Identifier for this Crawler (i.e. hostname)
        std::vector<url::URL*> work_unit; // Units of work (URLs to crawl)
};

struct RankCrawler : std::binary_function<Crawler, Crawler, bool>
{
    bool operator() (const Crawler *lhs, const Crawler *rhs) const;
};

} // sched
} // oodles

#endif
