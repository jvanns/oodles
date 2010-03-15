#ifndef OODLES_URL_TREE_HPP // Interface
#define OODLES_URL_TREE_HPP

// oodles
#include "Node.hpp"

// STL
#include <vector>

namespace oodles {
namespace url {

template<class T>
class Tree
{
    public:
        typedef typename Node<T>::tree_index_t tree_index_t;
        typedef typename Node<T>::path_index_t path_index_t;
    public:
        Tree();

        template<class Iterator>
        tree_index_t insert(Iterator b, // Beginning
                            Iterator e, // End
                            tree_index_t i = invalid_index); // Parent index
    private:
        tree_index_t insert(const T &v, path_index_t i, tree_index_t i);
    private:
        Node<T> root;
        std::vector<Node<T> > nodes;
        static const tree_index_t invalid_index; 
};

} // url
} // oodles

#include "Tree.ipp" // Implementation

#endif

