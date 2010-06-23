#ifndef OODLES_SCHED_PAGEDATA_HPP
#define OODLES_SCHED_PAGEDATA_HPP

// oodles
#include "url/URL.hpp"

#include <stdint.h> // For uint32_t

namespace oodles {
namespace sched {

class Node; // Forward declaration for PageData
class Crawler; // Forward declaration for PageData

struct PageData
{
    url::URL url; // Our URL object/structure.
    Crawler *crawler; // Our crawler, if any.
    const Node *referrer; // Link to a Node that holds a hyperlink to us.

    time_t last_crawl; // Time of last crawl or 0
    const time_t epoch; // Creation time of PageData

    uint32_t links, // No. of times this page is 'seen'
             crawl_count; // No. of times this page has been crawled

    PageData(const std::string &url);
    void assign_crawler(Crawler *c);
};

} // sched
} // oodles

#endif
