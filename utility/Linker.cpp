// oodles
#include "Linker.hpp"

// libc
#include <stdio.h> // For NULL

namespace oodles {

// Linker
Linker::Linker() : coupling(NULL)
{
}

Linker::~Linker()
{
    unlink();
}

bool
Linker::extend_link_to(Linker *l)
{
    if (!coupling)
        return false;

    const Link link(l, coupling->complement_of(*this));

    return true;
}

void
Linker::unlink()
{
    if (coupling) {
        coupling->left = coupling->right = NULL;
        delete coupling;
        coupling = NULL;
    }
}

void
Linker::link(Link *l)
{
    unlink();
    coupling = l;
}

// Link
Link::Link(Linker *left, Linker *right) : left(left), right(right)
{
    this->left->link(new Link(*this));
    this->right->link(new Link(*this));
}

Link::Link(const Link &l) : left(l.left), right(l.right)
{
}

Link&
Link::operator= (const Link &l)
{
    if (this != &l) {
        left = l.left;
        right = l.right;
    }

    return *this;
}

Linker*
Link::complement_of(const Linker &source) const
{
    if (left == &source)
        return right;

    if (right == &source)
        return left;

    return NULL;
}

} // oodles
