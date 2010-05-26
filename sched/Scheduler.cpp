// oodles
#include "PageData.hpp"
#include "Scheduler.hpp"

#include <time.h> // For time()

// STL
using std::string;
using std::vector;

static
inline
oodles::sched::Node*
parent_of(oodles::sched::Node::Base &node)
{
    return static_cast<oodles::sched::Node*>(node.parent);
}

namespace oodles {
namespace sched {

Scheduler::Scheduler() : leaves(0), tree(new Node("ROOT"))
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
    size_t i = 0, j = crawlers.size();
    for (Crawler *c = crawlers.top() ; i < j ; c = crawlers.top(), ++i) {
        if (c->online()) // Do not assign anything to offline Crawlers
            fill_crawler(*c); // Assign as much work to crawler (fill work unit)

        crawlers.pop(); // Pop the top of the queue (i.e. remove 'c')
        crawlers.push(c); // Push c back onto the queue forcing rank order
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
Scheduler::traverse_branch(Node &n) const
{
    Node *p = select_best_node(n);

    /*
     * If we have been unable to consider any children as eligible for
     * crawling then we must mark the node as 'Red' - we do not
     * want to traverse it again... yet.
     */
    if (!p) {
        if (n.eligible()) // Return when we've found something crawlable
            return &n;

        p = parent_of(n); // Begin to backtrack

        if (!p)
            return NULL; // We've returned to the root node
    }

    return traverse_branch(*p); // Continue delving deeper
}

void
Scheduler::weigh_tree_branch(Node &n) const
{
    if (!n.parent)
        return;

    Node *parent = parent_of(n);

    parent->weight -= n.weight;
    n.weight = n.calculate_weight();
    parent->weight = parent->weight + (n.weight / (n.size() + 1));

    weigh_tree_branch(*parent);
}

Node*
Scheduler::select_best_node(Node &parent) const
{
    Node *n = NULL;

    for (size_t i = 0 ; i < parent.size() ; ++i) {
        Node &c = parent.child(i);

        if (c.visit_state == Node::Red) // Skip-over any visited branch
#ifdef DEBUG_SCHED
        {
            std::cerr << '[' << c.value << "]: marked RED\n";
#endif
            continue;
#ifdef DEBUG_SCHED
        }
#endif

        c.visit_state = Node::Amber; // Node visited but not all children

        if (c.page && !c.eligible()) // Ignore ineligible yet crawlable nodes
            continue;

        if (!n)
            n = &c;
        else if (c.weight > n->weight)
            n = &c;
    }

    if (!n) // No child was a candidate although all were considered
        parent.visit_state = Node::Red;

#ifdef DEBUG_SCHED
    if (n)
        std::cerr << '[' << n->value << "]: Selected\n";
#endif

    return n;
}

void
Scheduler::fill_crawler(Crawler &c)
{
    Node *n = NULL, *p = NULL;
    static const Node *root = static_cast<const Node*>(&tree.root());

    if (root->visit_state == Node::Red) // Every single node has been traversed
#ifdef DEBUG_SCHED
    {
        std::cerr << "[ROOT]: Entire tree marked RED!\n";
#endif
        return;
#ifdef DEBUG_SCHED
    }
#endif

    while (c.unit_size() < Crawler::max_unit_size()) {
        if (!n)
            n = const_cast<Node*>(root); // At the top of the tree
        else
            n = parent_of(*n); // Begin to backtrack

        if (!n)
            break; // We've exhausted the tree (root's parent is NULL)

        p = n; // Keep a copy of our current position
        n = traverse_branch(*n); // Traverse to find best candidate for crawling

        if (n && n->eligible())
            n->page->assign_crawler(&c);
        else
            n = p; // Restore the copy of the previous node
    }
}

void
Scheduler::schedule(const string &url, bool from_seed)
{
    bool duplicate = true;
    PageData *page = new PageData(url);
    Node *node = static_cast<Node*> (tree.insert(page->url.begin_tree(),
                                                 page->url.end_tree()));

    if (node->leaf() && !node->page)
        duplicate = false;

    if (!duplicate) { // Newly inserted, unique URL
        ++leaves;
        node->page = page; // Ownership of page is implicitly transferred here
#ifdef DEBUG_SCHED
        std::cerr << '[' << leaves << "]: Leaves (pages) stored\n";
#endif
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
