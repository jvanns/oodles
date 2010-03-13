#ifndef OODLES_URL_TREE_HPP // Interface
#define OODLES_URL_TREE_HPP

// oodles
#include "Node.hpp"

// STL
#include <vector>
#include <iterator>

namespace oodles {
namespace url {

template<class T>
class Tree
{
    public:
        typedef Node<T>::tree_index_t index_t;
    public:
        Tree();
        ~Tree();

        index_t insert(std::iterator &b, // Beginning
                       std::iterator &e, // End
                       index_t i = no_index); // Parent index
    private:
        index_t insert(const T &v, Node<T>::path_index_t i, index_t i);
    private:
        const Node<T> root;
        std::vector<Node<T> > nodes;
        static const index_t no_index; 
};

} // url
} // oodles

#include "Tree.ipp" // Implementation

#endif

