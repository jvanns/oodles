// oodles
#include "Context.hpp"
#include "PageData.hpp"
#include "Scheduler.hpp"
#include "DeferredUpdate.hpp"
#include "utility/BreadCrumbTrail.hpp"

#include <time.h> // For time()

// STL
using std::string;
using std::vector;
using std::ostream;

static
inline
oodles::sched::Node*
parent_of(oodles::sched::Node::Base &node)
{
    return static_cast<oodles::sched::Node*>(node.parent);
}

namespace oodles {
namespace sched {

Scheduler::Scheduler(Context *c) :
    ctxt(c),
    leaves(0),
    trail(NULL),
    update(NULL),
    tree(new Node("ROOT"))
{
    if (c)
        update = new DeferredUpdate(c->proactor());
}

Scheduler::~Scheduler()
{
    delete update;
}

uint32_t
Scheduler::run(BreadCrumbTrail *t)
{
    if (crawlers.empty())
        return 0; // Short-cut! Don't bother continuing if 0 crawlers.
    
    /*
     * In our priority queue, crawlers, the top most item will
     * be the one with the least amount of work (therefore able
     * to take on more). If that crawler has reached full capacity
     * we can do no more.
     */
    Node *n = NULL;
    vector<Crawler*> deferred_crawls;
    uint32_t i = 0, j = crawlers.size(), k = 0, l = 0;

    trail = t; // Set the BCT, if any
    deferred_crawls.reserve(j); // Avoid potential (re)allocations

    for (Crawler *c = crawlers.top() ; i < j ; c = crawlers.top(), ++i) {
        if (c->online()) { // Do not assign anything to offline Crawlers
            l = fill_crawler(*c, n); // Assign as much work (fill work unit)

            if (l) {
                k += l;
                deferred_crawls.push_back(c); // Don't send messages just yet...
            }
        }

        crawlers.pop(); // Pop the top of the queue (i.e. remove 'c')
        crawlers.push(c); // Push c back onto the queue forcing rank order
    }

    trail = NULL; // Clear the BCT

    for (i = 0, j = deferred_crawls.size() ; i < j ; ++i)
        deferred_crawls[i]->begin_crawl();

    return k;
}

uint32_t
Scheduler::update_schedule()
{
    assert(update);
    return update->update(*this);
}

void
Scheduler::update_node(url::URL::hash_t id, time_t time)
{
    Node *n = page_table[id];
    PageData *p = n->page;

    p->last_crawl = time;
    ++p->crawl_count;

    p->unassign_crawler();
    weigh_tree_branch(*n);
}

void
Scheduler::defer_update(const Deferable &d, event::Subscriber &s)
{
    assert(update);
    update->defer(d, s);
}

url::URL::hash_t
Scheduler::schedule_from_seed(const string &url)
{
    return schedule(url, true);
}

url::URL::hash_t
Scheduler::schedule_from_crawl(const string &url)
{
    return schedule(url, false);
}

Node*
Scheduler::traverse_branch(Node &n)
{
    Node *p = select_best_child(n);

    if (trail)
        trail->drop_crumb(n.path_idx, n.child_idx); // Leave a breadcrumb trail

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
Scheduler::select_best_child(Node &parent) const
{
    Node *n = NULL;

    /*
     * Linear search over the breadth of this parent branch
     */
    for (size_t i = 0 ; i < parent.size() ; ++i) {
        Node &c = parent.child(i);

        if (c.visit_state == Node::Red) // Skip-over any visited branch
            continue;

        c.visit_state = Node::Amber; // Node visited but not all children

        if (c.page && !c.eligible()) // Ignore ineligible yet crawlable nodes
            continue;

        if (!n || (n && c.weight > n->weight))
            n = &c;
    }

    if (!n) { // No child was a candidate although all were considered
        parent.visit_state = Node::Red;

        if ((n = parent_of(parent)))
            ++n->visited; // Update the grandparent

        n = NULL;
    }

    return n;
}

Crawler::unit_t
Scheduler::fill_crawler(Crawler &c, Node *&n)
{
    static const Node &const_root = static_cast<const Node&>(tree.root());
    static Node *root = const_cast<Node*>(&const_root);

    Node *p = NULL;
    Crawler::unit_t assigned = 0;
    bool exhausted = root->visit_state == Node::Red;

    for (n = !n ? root : n ; !exhausted ; n = !n ? root : parent_of(*n)) {
        if (!(exhausted = !n || c.full())) {
            p = n; // Keep a copy of our current position
            n = traverse_branch(*n); // Locate best candidate for crawling

            if (n && n->eligible()) {
                page_table[n->page->url.page_id()] = n; // Cache it
                n->page->assign_crawler(&c);
                ++assigned;
            } else if (!n) {
                exhausted = true; // traverse_branch() exhausted the tree
            } else {
                n = p; // Restore the copy of the previous node
            }
        }
    }

    /*
     * If our last node was a leaf node that was assigned to
     * a crawler, mark it's ancestors as Red where necessary.
     */
    if (n && n->leaf() && (n->page && n->page->crawler)) {
        for (p = parent_of(*n) ; p ; ) {
            if (p->visited < p->size()) 
                break; // Terminate the loop if any unvisited children

            p->visit_state = Node::Red;

            if ((p = parent_of(*p)))
                ++p->visited; // Update the grandparent
        }
    }

    return assigned;
}

url::URL::hash_t
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

    assert(page);

    if (!from_seed) {
        ++page->links; // If we're from a seed it doesn't count as a link!
        weigh_tree_branch(*node); // Calculates branch schedule index (weight)
    }

    return page->url.page_id();
}

} // sched
} // oodles
