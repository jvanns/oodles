#ifndef OODLES_TREE_IPP // Implementation
#define OODLES_TREE_IPP

namespace oodles {

template<class Type>
void
Tree<Type>::print(std::ostream &stream) const
{
    stream << root();
}

/*
 * Insert a range in-sequence in the tree;
 * b = Beginning of range
 * e = End of range
 * p = Node of parent returned from last call
 * i = Path ID of current item (dereferenced iterator)
 */
template<class Type>
template<class Iterator>
Node<Type>*
Tree<Type>::insert(Iterator b, Iterator e, Node<Type> *p)
{
    path_index_t i = 0;

    if (p)
        i = p->path_idx + 1;
    else
        p = &seed;

    while (b != e) {
        p = insert(*b, i, *p);
        ++i;
        ++b;
    }

    return p;
}

template<class Type>
void
Tree<Type>::depth_first_traverse(Node<Type> &n) const
{
    visit(n);

    if (n.leaf())
        return;

    typename Node<Type>::iterator i = n.children.begin(), j = n.children.end();
    for ( ; i != j ; ++i)
        depth_first_traverse(*(*i));
}

template<class Type>
void
Tree<Type>::breadth_first_traverse(Node<Type> &n) const
{
    if (n.leaf())
        return;

    typename Node<Type>::iterator i = n.children.begin(), j = n.children.end();
    for ( ; i != j ; ++i)
        visit(*(*i));

    visit(n);

    /* FIXME: Pretty lame having to loop twice! */
    for (i = n.children.begin() ; i != j ; ++i)
        breadth_first_traverse(*(*i));
}

template<class Type>
inline
void
Tree<Type>::visit(Node<Type> &n) const
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
template<class Type>
Node<Type>*
Tree<Type>::insert(const Type &v, path_index_t i, Node<Type> &p)
{
    Node<Type> *c = NULL;

    if (!p.has_child(v, c))
        c = p.create_child(v, i); // Create new child node

    return c; // Return it to caller so it can be passed back here as a parent
}

} // oodles

#endif

