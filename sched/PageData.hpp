#ifndef OODLES_SCHED_PAGEDATA_HPP
#define OODLES_SCHED_PAGEDATA_HPP

// oodles
#include "url/URL.hpp"

namespace oodles {
namespace sched {

class Node; // Forward declaration for PageData

struct PageData
{
    const url::URL url;
    const Node *referrer;

    time_t epoch, last_crawl;
    uint32_t links, crawl_count;

    PageData(const std::string &url);
};

} // sched
} // oodles

#endif
