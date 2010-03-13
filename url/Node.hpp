#ifndef OODLES_URL_NODE_HPP // Interface
#define OODLES_URL_NODE_HPP

#include <vector>

#include <stdint.h> // For uint16_t

namespace oodles {
namespace url {

template<class T>
struct Node
{
    typedef T value_t;
    typedef uint16_t path_index_t;
    typedef uint16_t node_index_t;
    typedef uint32_t tree_index_t;

    value_t value;
    path_index_t path_id; // Index of this node within it's path
    node_index_t node_id; // Index of this node amoung it's siblings
    tree_index_t tree_id; // Index of this node according to the whole tree

    Node *parent;
    std::vector<Node<T>*> children;

    bool has_child(const T &v, Node<T> *&c) const;
};

} // url
} // oodles

#include "Node.ipp" // Implementation

#endif

