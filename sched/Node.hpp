#ifndef OODLES_SCHED_NODE_HPP
#define OODLES_SCHED_NODE_HPP

// oodles
#include "url/URL.hpp"
#include "utility/Node.hpp"

namespace oodles {
namespace sched {

class PageData; // Forward declration for Node

class Node : public oodles::Node<url::value_type>
{
    public:
        /* Dependent typedefs */
        typedef url::value_type value_type;

        /* Member functions/methods */
        Node(const value_type &v);
        ~Node();

        /* Member variables/attributes */
        float weight; // Weight of this *branch* inc. this node
        PageData *page; // Only used with leaf nodes, NULL otherwise
    private:
        /* Member functions/methods */
        void visit();
        Node* new_node(const value_type &v) const;
};

} // sched
} // oodles

#endif

