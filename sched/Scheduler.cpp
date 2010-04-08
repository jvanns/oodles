// oodles
#include "PageData.hpp"
#include "Scheduler.hpp"

#include <time.h> // For time()

// STL
using std::string;
using std::vector;

namespace oodles {
namespace sched {

Scheduler::Scheduler() : leaves(0)
{
}

Scheduler::~Scheduler()
{
}

uint32_t
Scheduler::run()
{
    /*
     * In our priority queue, crawlers, the top most item will
     * be the one with the least amount of work (therefore able
     * to take on more). If that crawler has reached full capacity
     * we can do no more.
     */
    Crawler &c = *(crawlers.top());
    for (size_t i = 0, j = crawlers.size() ; i < j ; ++i) {
        fill_crawler(c); // Assign as much work to crawler (fills work unit)

        crawlers.pop(); // Pop the top of the queue (i.e. remove 'c')
        crawlers.push(&c); // Push c back onto the queue forcing rank order
    }

    return 0;
}

bool
Scheduler::register_crawler(Crawler &c)
{
    crawlers.push(&c); // TODO: Check for duplicates
    return true;
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

inline
Node*
Scheduler::traverse_branch(Node *n) const
{
    if (!n)
        return NULL;

    return traverse_branch(select_best_node(*n));
}

void
Scheduler::weigh_tree_branch(Node &n) const
{
    if (!n.parent)
        return;

    Node &parent = static_cast<Node&> (*(n.parent));

    parent.weight -= n.weight;
    n.weight = n.calculate_weight();
    parent.weight = parent.weight + (n.weight / (n.children.size() + 1));

    weigh_tree_branch(parent);
}

Node*
Scheduler::select_best_node(const Node &parent) const
{
    Node *c = NULL, *n = NULL;
    vector<Node::Base*>::const_iterator i = parent.children.begin();

    for ( ; i != parent.children.end() ; ++i) {
        c = static_cast<Node*>(*i);

        if (c->leaf() && !c->eligible()) // Ignore any ineligible leaf nodes
            continue;

        if (!n)
            n = c;
        else if (c->weight > n->weight)
            n = c;
    }

    return n;
}

void
Scheduler::fill_crawler(Crawler &c)
{
    Node *n = NULL, *p = NULL;
    static const Node *root = static_cast<const Node*>(&tree.root());

    while (c.unit_size() < Crawler::max_unit_size()) {
        if (!n)
            n = const_cast<Node*>(root);
        else
            p = n = static_cast<Node*>(n->parent);

        if (!n)
            break; // We've returned to the top of the tree, exhausted

        n = traverse_branch(n);

        if (n && n->eligible())
            n->page->assign_crawler(&c);
        else
            n = p; // Begin to backtrack up the tree
    }
}

void
Scheduler::schedule(const string &url, bool from_seed)
{
    PageData *page = new PageData(url);
    Node *node = static_cast<Node*> (tree.insert(page->url.begin_tree(),
                                                 page->url.end_tree()));

    if (!node->page) { // Newly inserted, unique URL
        ++leaves;
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

} // sched
} // oodles
