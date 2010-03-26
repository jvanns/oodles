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
        typedef uint16_t path_index_t;
        typedef uint16_t child_index_t;

        /* Member functions/methods */
        bool leaf() const;
        void print(std::ostream &stream) const;
        bool has_child(const T &v, Node *&c) const;
        Node* create_child(const T &v, path_index_t i);

        /* Member variables/attributes */
        const T value; // Value at this node
        path_index_t path_idx; // Index of this node within it's path
        child_index_t child_idx; // Index of this node amoung it's siblings

        Node *parent; // Pointer to parent node
        std::vector<Node<T>*> children; // This nodes children
    protected:
        /* Dependent typedefs */
        typedef typename std::vector<Node<T>*>::const_iterator iterator;

        /* Member functions/methods */
        virtual ~Node();
        Node(const T &v);

        virtual void visit();
        virtual Node* new_node(const T &v) const;
    private:
        /* Member functions/methods */
        Node();

        Node(const Node &n); // Do not allow...
        Node& operator=(const Node &n); // ... copying presently.

        bool find(const size_t l, const size_t r, const T &v, size_t &n) const;

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

