#ifndef OODLES_NODE_HPP // Interface
#define OODLES_NODE_HPP

// oodles
#include "NodeBase.hpp"
#include "IndexedSet.hpp"

// STL
#include <functional>

namespace oodles {

template<class T> class Tree; // Forward declaration for friend below

template<class T>
class Node : public NodeBase
{
    public:
        /* Member functions/methods */
        Node* create_child(const T &v, path_index_t i);

        NodeBase& child(size_t i) { return *children[i]; }
        const NodeBase& child(size_t i) const { return *children[i]; }

        size_t size() const { return children.size(); }
        void print(std::ostream &stream) const { stream << value; }

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
        struct KeyCmp : public std::binary_function<NodeBase, NodeBase, bool>
        {
            bool operator() (const NodeBase *lhs, const NodeBase *rhs) const
            {
                const Node<T> &l = *lhs, &r = *rhs;
                return l.value < r.value;
            }
        };

        /* Member variables/attributes */
        IndexedSet<NodeBase*, KeyCmp> children;

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
