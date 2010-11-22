// oodles
#include "Node.hpp"

// libc
#include <math.h> // For fabs()

// STL
using std::ostream;

namespace {

/*
 * Implementation of normalisation formula presented here:
 * http://people.revoledu.com/kardi/tutorial/Similarity/Normalization.htm
 */
inline
double
normalise(double weight, double minimum)
{
    if (minimum < 0)
        weight += fabs(minimum); // Ensure all data remains positive 
    
    return 0.5f * (1 - (weight / sqrt(pow(weight, 2) + 1000)));
}

} // anonymous

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
            ptrdiff_t nid = reinterpret_cast<ptrdiff_t>(this);
            s << nid << " [label=\"" << value << "\", color=blue];\n";
        }
    } else {
        s << value;
    }
}

double
Node::weight() const
{
    return normalise(measure.current / (size() + 1), measure.minimum);
}

/*
 * TODO: Formula untested. Must write crawl simulation to test
 * scoring/weighting math. May be a simpler (+/- 1) solution?
 */
void
Node::calculate_weight()
{
    if (!parent)
        return;

    Node &p = *parent;

    /*
     * The #links a page has (to it) is a measure of it's popularity so
     * we use it as a multiplier to suggest to the scheduler it may be
     * more important to keep 'fresh' than others. However, to ensure this
     * doesn't get favoured to often we use the #crawls against it as a
     * divisor to bring it's weight back down.
     */
    if (page) {
        double t = page->epoch, c = page->crawl_count + 1, l = page->links + 1;
        double new_weight = (t / c) * l, old_weight = measure.current;

        measure.current = new_weight + (leaf() ? 0 : old_weight);
        measure.previous = old_weight;

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

    /*
     * We must keep a record of the minimum measure as we go up the tree as
     * we need to pass this to normalise() in order to maintain a positive
     * set of numbers to transform (to a value between 0 and 1).
     */
    if (measure.current < measure.minimum)
        measure.minimum = measure.current;

    if (measure.minimum < p.measure.minimum)
        p.measure.minimum = measure.minimum;
}

Node*
Node::weigh_against(Node *&n)
{
    if (n && n->weight() > weight())
        return n;

    return this;
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

