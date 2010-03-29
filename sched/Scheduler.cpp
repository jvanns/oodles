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

uint32_t
Scheduler::run()
{
    return 0;
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
Scheduler::weigh_tree_branch(Node &n) const
{
    if (!n.parent)
        return;

    Node &parent = static_cast<Node&> (*(n.parent));

    /*
     * TODO: Formula untested - simulate a non-seeded set of URLs
     */
    if (n.leaf()) {
        parent.weight -= n.weight;
        n.weight = (n.page->links *
                   (time(NULL) - (n.page->last_crawl - n.page->epoch)))
                   / n.page->crawl_count + 1;
    }

    parent.weight = parent.weight + (n.weight / (n.children.size() + 1));
    weigh_tree_branch(parent);
}

void
Scheduler::schedule(const string &url, bool from_seed)
{
    PageData *page = new PageData(url);
    Node *node = static_cast<Node*> (tree.insert(page->url.begin_tree(),
                                                 page->url.end_tree()));

    if (!node->page) { // Newly inserted, unique URL
        page->epoch = time(NULL);
        node->page = page; // Ownership of page is implicitly transferred here
    } else {
        delete page;
        page = node->page;
    }

    if (!from_seed) {
        ++page->links; // If we're from a seed it doesn't count as a link!
        weigh_tree_branch(*node); // Calculates branch schedule index (weight)
    }
}

}
}

