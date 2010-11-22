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

        /* Override print() method from NodeBase */
        void print(std::ostream &s, const io::PrinterBase &p) const;
        
        double weight() const;
        void calculate_weight();
        Node* weigh_against(Node *&n);

        bool assigned() const { return page && page->crawler; }
        bool eligible() const { return page ? page->crawler == NULL : false; }

        /* Member variables/attributes */
        PageData *page; // Only used with leaf nodes, NULL otherwise
        size_t visited; // Keep an index/tally of visited children
    private:
        /* Member functions/methods */
        void visit();
        Node* new_node(const value_type &v) const;

        struct Measure {
            double minimum, previous, current;
            Measure() : minimum(0), previous(0), current(0) {}
        };

        /* Member variables/attributes */
        Measure measure, *reviser;
};

} // sched
} // oodles

#endif

