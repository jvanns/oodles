// oodles
#include "PageData.hpp"
#include "Scheduler.hpp"

#include <time.h> // For time()

// STL
using std::string;

namespace oodles {
namespace sched {

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

void
Scheduler::schedule_from_seed(const string &url)
{
    schedule(url, true);
}

void
Scheduler::schedule_from_crawl(const string &url)
{
    schedule(url, false);
}

void
Scheduler::schedule(const string &url, bool from_seed)
{
    PageData *page = new PageData(url);
    Node *node = static_cast<Node*> (tree.insert(page->url.begin_tree(),
                                                 page->url.end_tree()));

    assert(node->leaf()); // The returned node should never have children

    if (!node->page) { // Newly inserted, unique URL
        page->epoch = time(NULL);
        node->page = page; // Ownership of page is implicitly transferred here
    } else {
        delete page;
        page = node->page;
    }

    if (!from_seed)
        ++page->links; // If we're from a seed it doesn't count as a link!
}

}
}

