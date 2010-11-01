// oodles
#include "Node.hpp"
#include "utility/math.hpp"

// STL
using std::ostream;

namespace oodles {
namespace sched {

// Public methods

Node::Node(const value_type &v) :
    oodles::Node<value_type>(v),
    weight(0.0f),
    page(NULL),
    visited(0)
{
}

Node::~Node()
{
    delete page; // If set, ownership is implicitly transferred
}

void
Node::print(ostream &s, const io::PrinterBase &p) const
{
    if (typeid(p) == typeid(io::DotMatrix)) {
        if (!assigned()) {
            oodles::Node<value_type>::print(s, p);
        } else {
            /*
             * Declare a node (using the pointer address as the ID)
             */
            const ptrdiff_t nid = reinterpret_cast<ptrdiff_t>(this);
            s << nid << " [label=\"" << value << "\", color=blue];\n";
        }
    } else {
        s << value;
    }
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
Node::calculate_weight(time_t now) const
{
    if (!page)
        return 0.0f;

    float creat_diff = now - page->epoch,
          crawl_diff = page->last_crawl ?
               now - page->last_crawl : 0,
          max = crawl_diff ? crawl_diff : creat_diff,
          min = !crawl_diff ? crawl_diff : creat_diff,
          score = ((page->links + 1) * (max - min)) / (page->crawl_count + 1);

    return normalise<float> (score, 0.0f, 1.0f, min, max);
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

