#ifndef OODLES_NODE_HPP // Interface
#define OODLES_NODE_HPP

// STL
#include <vector>
#include <iostream>

#include <stdint.h> // For uint16_t

namespace oodles {

template<class T> class Tree; // Forward declaration for friend below

template<class T>
class Node
{
    public:
        /* Dependent typedefs */
        typedef uint16_t path_index_t; // Depth
        typedef uint32_t child_index_t; // Breadth

        /* Member functions/methods */
        bool leaf() const;
        void print(std::ostream &stream) const;
        bool has_child(const T &v, Node *&c) const;
        Node* create_child(const T &v, path_index_t i);

        /* Member variables/attributes */
        const T value; // Value at this node
        path_index_t path_idx; // Index of this node within it's path
        child_index_t child_idx; // Index of this node amoung it's siblings

        /* Visitation state */
        enum {
            Black = 0, // This node not visited nor any of it's children
            White = 1, // This node and all children have been visited
            Grey = 2 // This node visited but not yet all children
        };

        Node *parent; // Pointer to parent node
        int visit_state; // Visitation state for this node
        std::vector<Node*> children; // This nodes children
    protected:
        /* Dependent typedefs */
        typedef typename std::vector<Node*>::const_iterator iterator;

        /* Member functions/methods */
        Node(const T &v);
        virtual ~Node();

        virtual void visit();
        virtual Node* new_node(const T &v) const;
    private:
        /* Member functions/methods */
        Node();

        Node(const Node &n); // Do not allow...
        Node& operator=(const Node &n); // ... copying presently.

        bool find(size_t l, size_t r, const T &v, size_t &n) const;
        bool bsearch(size_t l,
                     size_t r,
                     const T &v,
                     size_t &n,
                     const bool &cancel) const;

        /* Friend class declarations */
        friend class Tree<T>; // Tree wants Node() only.
};

template<class T>
static
inline
std::ostream&
operator<< (std::ostream &stream, const Node<T> &node)
{
    node.print(stream);
    return stream;
}

} // oodles

#include "Node.ipp" // Implementation

#endif

