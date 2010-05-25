#ifndef OODLES_NODE_HPP // Interface
#define OODLES_NODE_HPP

// oodles
#include "NodeBase.hpp"

// STL
#include <functional>

#include <stdint.h> // For uint16_t

namespace oodles {

template<class T> class Tree; // Forward declaration for friend below

template<class T>
class Node : public NodeBase
{
    public:
        /* Member functions/methods */
        void print(std::ostream &stream) const;
        Node* create_child(const T &v, path_index_t i);

        /* Member variables/attributes */
        const T value; // Value at this node
    protected:
        /* Member functions/methods */
        Node(const T &v);
        ~Node();

        virtual void visit();
        virtual Node* new_node(const T &v) const;
    private:
        /* Internal Data Structures */
        struct KeyCmp : public std::binary_function<NodeBase, T, bool>
        {
            bool operator() (const NodeBase *lhs, const T &rhs) const
            {
                const Node<T> &n = *lhs;
                return n.value < rhs;
            }
        };

        /* Member functions/methods */
        Node();
        Node(const Node &n); // Do not allow...
        Node& operator=(const Node &n); // ... copying presently.

        /* Friend class declarations */
        friend class Tree<T>; // Tree wants Node() only.
};

} // oodles

#include "Node.ipp" // Implementation

#endif
