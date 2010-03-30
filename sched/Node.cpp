// oodles
#include "Node.hpp"
#include "PageData.hpp"

namespace oodles {
namespace sched {

// Public methods

Node::Node(const value_type &v) :
    oodles::Node<value_type>(v),
    weight(0.0f),
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

inline
Node*
Node::new_node(const value_type &v) const
{
    return new sched::Node(v); // Fully qualify the Node type we're creating
}

} // sched
} // oodles

