#ifndef OODLES_URL_TREE_IPP // Implementation
#define OODLES_URL_TREE_IPP

namespace oodles {
namespace url {

template<class T>
const Tree<T>::index_t Tree<T>::no_index = (Tree<T>::index_t) -1;

template<class T>
Tree<T>::Tree()
{
}

/*
 * Insert a range in-sequence in the tree;
 * b = Beginning of range
 * e = End of range
 * i = Index of parent returned from last call
 * p = Path ID of current item (dereferenced iterator)
 */
template<class T>
Tree<T>::index_t
Tree<T>::insert(std::iterator &b, std::iterator &e, index_t i)
{
    Node<T>::path_index_t p = (i != no_index ? nodes[i].path_id + 1 : 0);

    while (b != e) {
        i = insert(*b, p, i);
        ++p;
        ++b;
    }
}

/*
 * Insert an individual item from the range above;
 * v = Item value (i.e. 'com' or 'www' etc.)
 * p = Path index ID
 * i = Index of parent node
 * c = Current node by index i
 * t = Temporary pointer
 */
template<class T>
Tree<T>::index_t
Tree<T>::insert(const T &v, Node<T>::path_index_t p, index_t i)
{
    Node<T> *c = &(i != no_index ? nodes[i] : root), *t = NULL;

    if (c->has_child(v, t))
        return t->tree_id;

    nodes.push_back(Node<T>(v, p)); // Create new node; store it in the vector

    t = &nodes.back(); // Take the address of it

    t->parent = c; // Set it's parent
    t->tree_id = nodes.size() - 1; // Assign the array index to it
    t->node_id = c->children.size(); // Assign this sibling's index

    c->children.push_back(t); // Store a pointer to t as (it is) a child of c

    return t->tree_id;
}

} // url
} // oodles

#endif

