#ifndef OODLES_SCHED_NODE_HPP
#define OODLES_SCHED_NODE_HPP

// oodles
#include "PageData.hpp"
#include "utility/Node.hpp"

namespace oodles {
namespace sched {

class PageData; // Forward declration for Node

class Node : public oodles::Node<url::value_type>
{
    public:
        /* Dependent typedefs */
        typedef url::value_type value_type;
        typedef oodles::Node<value_type> Base;

        /* Member functions/methods */
        Node(const value_type &v);
        ~Node();

        float calculate_weight() const;
        bool eligible() const { return page ? page->crawler == NULL : false; }

        /* Member variables/attributes */
        float weight; // Weight of this *branch* inc. this node
        PageData *page; // Only used with leaf nodes, NULL otherwise
        child_index_t visited; // Keep an index/tally of visited children
    private:
        /* Member functions/methods */
        void visit();
        Node* new_node(const value_type &v) const;
};

struct RankNode : public std::binary_function<Node::Base, Node::Base, bool>
{
    bool operator() (const Node::Base *lhs, const Node::Base *rhs) const;
};

} // sched
} // oodles

#endif

