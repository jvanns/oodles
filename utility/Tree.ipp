#ifndef OODLES_TREE_IPP // Implementation
#define OODLES_TREE_IPP

namespace oodles {

template<class T>
void
Tree<T>::print(std::ostream &stream) const
{
    stream << root;
}

/*
 * Insert a range in-sequence in the tree;
 * b = Beginning of range
 * e = End of range
 * p = Node of parent returned from last call
 * i = Path ID of current item (dereferenced iterator)
 */
template<class T>
template<class Iterator>
Node<T>*
Tree<T>::insert(Iterator b, Iterator e, Node<T> *p)
{
    path_index_t i = 0;

    if (p)
        i = p->path_idx + 1;
    else
        p = &root;

    while (b != e) {
        p = insert(*b, i, *p);
        ++i;
        ++b;
    }

    return p;
}

/*
 * Insert an individual item from the range above;
 * v = Item value (i.e. 'com' or 'www' etc.)
 * i = Path index ID
 * p = Parent node
 * c = Current node
 */
template<class T>
Node<T>*
Tree<T>::insert(const T &v, path_index_t i, Node<T> &p)
{
    Node<T> *c = NULL;

    if (!p.has_child(v, c))
        c = p.create_child(v, i); // Create new child node

    return c;
}

} // oodles

#endif

