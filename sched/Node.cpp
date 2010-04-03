// oodles
#include "Node.hpp"
#include "PageData.hpp"
#include "utility/math.hpp"

namespace oodles {
namespace sched {

// Public methods

Node::Node(const value_type &v) :
    oodles::Node<value_type>(v),
    weight(0.0f),
    assigned(false),
    page(NULL)
{
}

Node::~Node()
{
    delete page; // If set, ownership is implicitly transferred
}

// Private methods

void
Node::visit()
{
}

/*
 * TODO: Formula untested. Must write crawl simulation to test
 * scoring/weighting math. May be a simpler (+/- 1) solution?
 */
float
Node::calculate_weight() const
{
    if (!page)
        return 0.0f;

    const time_t now = time(NULL),
                 max = std::max(now - page->epoch,
                                now - page->last_crawl),
                 min = std::min(now - page->epoch, max),
                 score = (page->links * (now - (max - min)))
                                    / page->crawl_count + 1;

    return normalise<time_t> (score, 0, 1, min, max);
}

inline
Node*
Node::new_node(const value_type &v) const
{
    return new sched::Node(v); // Fully qualify the Node type we're creating
}

bool
RankNode::operator() (const Node::Base *lhs, const Node::Base *rhs) const
{
    return static_cast<const Node*>(lhs)->weight <
           static_cast<const Node*>(rhs)->weight;
}

} // sched
} // oodles

