#ifndef OODLES_SCHED_NODE_HPP
#define OODLES_SCHED_NODE_HPP

// oodles
#include "url/URL.hpp"
#include "utility/Node.hpp"

namespace oodles {
namespace sched {

class Node : public oodles::Node<url::value_type>
{
    public:
        /* Dependent typedefs */
        typedef url::value_type value_type;

        /* Member functions/methods */
        Node(const value_type &v);
        ~Node();

        /* Member variables/attributes */
        time_t last_crawl;
        size_t crawl_count;

        const url::URL *url; // Associated URL for this (leaf) node
        const Node *referer; // Point to the Node who's page refered to this URL
    private:
        /* Member functions/methods */
        Node* new_node(const value_type &v) const;
};

} // sched
} // oodles

#endif

