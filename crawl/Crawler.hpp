#ifndef OODLES_CRAWL_CRAWLER_HPP
#define OODLES_CRAWL_CRAWLER_HPP

// oodles
#include "url/URL.hpp"
#include "utility/Linker.hpp"

// STL
#include <map>
#include <list>
#include <string>

namespace oodles {
namespace crawl {

class Crawler : public Linker
{
    public:
        /* Member functions/methods */
        Crawler(const std::string &name, uint16_t cores);

        uint16_t cores() const { return cpus; }
        const std::string& id() const { return name; }

        void fetch(const url::URL &url);
    private:
        /* Member variables/attributes */
        const uint16_t cpus;
        const std::string name;
        std::map<url::URL::hash_t, std::list<url::URL> > urls;
};

} // crawl
} // oodles

#endif

