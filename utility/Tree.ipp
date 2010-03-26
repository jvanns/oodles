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

template<class T>
void
Tree<T>::depth_first_traverse(Node<T> &n) const
{
    visit(n);

    if (n.leaf())
        return;

    typename Node<T>::iterator i = n.children.begin(), j = n.children.end();
    for ( ; i != j ; ++i)
        depth_first_traverse(*(*i));
}

template<class T>
void
Tree<T>::breadth_first_traverse(Node<T> &n) const
{
    if (n.leaf())
        return;

    typename Node<T>::iterator i = n.children.begin(), j = n.children.end();
    for ( ; i != j ; ++i)
        visit(*(*i));

    visit(n);

    /* FIXME: Pretty lame having to loop twice! */
    for (i = n.children.begin() ; i != j ; ++i)
        breadth_first_traverse(*(*i));
}

template<class T>
inline
void
Tree<T>::visit(Node<T> &n) const
{
    n.visit();
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

    return c; // Return it to caller so it can be passed back here as a parent
}

} // oodles

#endif

