#ifndef OODLES_URL_TREE_IPP // Implementation
#define OODLES_URL_TREE_IPP

namespace oodles {
namespace url {

template<class T>
const typename Tree<T>::tree_index_t Tree<T>::invalid_index = -1;

template<class T>
Tree<T>::Tree()
{
}

template<class T>
Tree<T>::~Tree()
{
    typedef typename std::vector<Node<T>*>::const_iterator iterator;

    for (iterator i = nodes.begin() ; i != nodes.end() ; ++i)
        delete *i;
}

/*
 * Insert a range in-sequence in the tree;
 * b = Beginning of range
 * e = End of range
 * i = Index of parent returned from last call
 * p = Path ID of current item (dereferenced iterator)
 */
template<class T>
template<class Iterator>
typename Tree<T>::tree_index_t
Tree<T>::insert(Iterator b, Iterator e, tree_index_t i)
{
    path_index_t p = i != invalid_index ? nodes[i]->path_id + 1 : 0;

    while (b != e) {
        i = insert(*b, p, i);
        ++p;
        ++b;
    }

    return i;
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
typename Tree<T>::tree_index_t
Tree<T>::insert(const T &v, path_index_t p, tree_index_t i)
{
    Node<T> *c = (i != invalid_index ? nodes[i] : &root), *t = NULL;

    if (c->has_child(v, t))
        return t->tree_id;
    
    t = new Node<T> (v, p); // Create new node

    t->parent = c; // Set it's parent
    t->tree_id = nodes.size() - 1; // Assign the array index to it
    t->node_id = c->children.size(); // Assign this sibling's index

    nodes.push_back(t); // Store a pointer to it in the vector
    c->children.push_back(t); // Store a pointer to t as (it is) a child of c

    return t->tree_id;
}

} // url
} // oodles

#endif

