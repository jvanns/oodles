#ifndef OODLES_TREE_HPP // Interface
#define OODLES_TREE_HPP

// oodles
#include "Node.hpp"

// STL
#include <vector>

namespace oodles {

template<class T>
class Tree
{
    public:
        typedef typename Node<T>::path_index_t path_index_t;
    public:
        template<class Iterator>
        Node<T>* insert(Iterator b, // Beginning
                        Iterator e, // End
                        Node<T> *p = NULL); // Parent node
    private:
        Node<T>* insert(const T &v, path_index_t i, Node<T> &p);
    private:
        Node<T> root;
};

} // oodles

#include "Tree.ipp" // Implementation

#endif

