// oodles
#include "Node.hpp"

namespace oodles {
namespace sched {

Node::Node(const value_type &v) :
    oodles::Node<value_type>(v),
    last_crawl(0),
    crawl_count(0),
    url(NULL),
    referer(NULL)
{
}

Node::~Node()
{
    delete url; // If set, ownership is implicitly transferred
}

inline
Node*
Node::new_node(const value_type &v) const
{
    return new sched::Node(v); // Fully qualify the Node type we're creating
}

} // sched
} // oodles

