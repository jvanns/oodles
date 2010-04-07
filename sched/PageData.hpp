#ifndef OODLES_SCHED_PAGEDATA_HPP
#define OODLES_SCHED_PAGEDATA_HPP

// oodles
#include "url/URL.hpp"

namespace oodles {
namespace sched {

class Node; // Forward declaration for PageData
class Crawler; // Forward declaration for PageData

struct PageData
{
    Crawler *crawler; // Our crawler, if any.
    const url::URL url; // Our URL object/structure.
    const Node *referrer; // Link to a Node that holds a hyperlink to us.

    time_t epoch, // Creation time of leaf node
           last_crawl; // Time of last crawl or 0
    uint32_t links, // No. of times this page is 'seen'
             crawl_count, // No. of times this page has been crawled
             paralleled; // No. of currently scheduled pages in same domain

    PageData(const std::string &url);
    void assign_crawler(Crawler *c);
};

} // sched
} // oodles

#endif
