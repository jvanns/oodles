// oodles
#include "Node.hpp"
#include "utility/math.hpp"

// STL
#include <limits>

// STL
using std::ostream;
using std::numeric_limits;

namespace oodles {
namespace sched {

// Public methods

Node::Node(const value_type &v) :
    oodles::Node<value_type>(v),
    page(NULL),
    visited(0),
    reviser(NULL)
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

double
Node::weight() const
{
    static const double min = numeric_limits<double>::min(),
                        max = numeric_limits<double>::max();

    const double d = ((measure.current + measure.considered) / (size() + 1));
        
    return normalise<double> (d, 0, 1, min, max);
}

Node*
Node::weigh_against(Node *&n)
{
    Node *x = this;

    /*
     * The 'considered' attribute of 'measure' is taken into consideration
     * the next time the weight() method is called and not now.
     */

    // FIXME: Doesn't this only need to be 'considered' when two nodes have an
    // equal weighting?
    if (!n || weight() > n->weight()) {
         measure.considered = -1; // Will decrement weighting by one

        if (n)
           n->measure.considered = 0;
    } else {
        x = n;
        measure.considered = 0;
        n->measure.considered = -1;
    }

    return x;
}

/*
 * TODO: Formula untested. Must write crawl simulation to test
 * scoring/weighting math. May be a simpler (+/- 1) solution?
 */
void
Node::calculate_weight(time_t now)
{
    if (!parent)
        return;

    Node &p = *parent;

    if (page) {
        size_t t = page->last_crawl ? (page->last_crawl - page->epoch) + 1 : 1,
               c = page->crawl_count + 1, // Divisor
               l = page->links + 1; // Multiplier

        // FIXME: What about non-leaf nodes that hold a page?
        measure.previous = measure.current;
        measure.current = ((t > 1 ? 1 / t : 1) * l) / c;

        if (measure.previous == measure.current)
            p.reviser = NULL;
        else
            p.reviser = &measure;
    } else {
        p.reviser = reviser;

        if (reviser) {
            measure.current += reviser->current;
            measure.current -= reviser->previous;
        }
    }
}

// Private methods

void
Node::visit()
{
}

inline
Node*
Node::new_node(const value_type &v) const
{
    return new sched::Node(v); // Fully qualify the Node type we're creating
}

} // sched
} // oodles

