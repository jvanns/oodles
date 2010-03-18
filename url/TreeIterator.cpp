// oodles
#include "URL.hpp"
#include "TreeIterator.hpp"

namespace oodles {
namespace url {

TreeIterator::TreeIterator(const URL *target, int state) :
    internal_state(state),
    external_state(-1),
    value(NULL),
    url(target),
    index(0)
{
    move_pointer_forward();
}

TreeIterator::TreeIterator(const TreeIterator &rhs)
{
    if (this != &rhs)
        *this = rhs;
}

TreeIterator&
TreeIterator::operator= (const TreeIterator &rhs)
{
    if (this != &rhs) {
        url = rhs.url;
        value = rhs.value;
        index = rhs.index;
        external_state = rhs.external_state;
        internal_state = rhs.internal_state;
    }

    return *this;
}

bool
TreeIterator::move_pointer_forward()
{
    bool recall = false;

    if (internal_state == Begin) {
        internal_state = Transient;
        external_state = URL::Domain;
        index = url->domain.size() - 1; // Prepare index for Domain state
    }

    switch (external_state) {
        case URL::Domain:
            if (url->domain.empty()) {
                external_state = URL::Path;
                recall = true;
            } else if (index >= 0) {
                value = &url->domain[index];

                if (index == 0)
                    external_state = URL::Path;
                else
                    --index;
            }
            break;
        case URL::Path:
            if (!url->path.empty() && index < url->path.size()) {
                value = &url->path[index];
                ++index;
            } else {
                external_state = URL::Page;
                recall = true;
                index = 0;
            }
            break;
        case URL::Page:
            value = &url->page;
            external_state = -1; // Finish
            break;
        default:
            value = NULL;
            internal_state = End;
            break;
    }

    return recall;
}

bool
TreeIterator::move_pointer_backward()
{
    bool recall = false;

    if (internal_state == End) {
        internal_state = Transient;
        external_state = URL::Page;
        index = url->path.size() - 1; // Prepare index for Path state
    }

    switch (external_state) {
        case URL::Domain:
            if (!url->domain.empty() && index < url->domain.size()) {
                value = &url->domain[index];
                ++index;
            } else {
                external_state = -1; // Finish
                recall = true;
                index = 0;
            }
            break;
        case URL::Path:
            if (url->path.empty()) {
                external_state = URL::Domain;
                recall =  true;
            } else if (index >= 0) {
                value = &url->path[index];

                if (index == 0)
                    external_state = URL::Domain;
                else
                    --index;
            }
            break;
        case URL::Page:
            value = &url->page;
            external_state = URL::Path;
            break;
        default:
            value = NULL;
            internal_state = End;
            break;
    }

    return recall;
}

TreeIterator&
TreeIterator::operator++()
{
    if (internal_state == Invalid) {
        value = NULL;
    } else if (internal_state != End) {
        while (move_pointer_forward());
    }

    return *this;
}

TreeIterator&
TreeIterator::operator--()
{
    if (internal_state == Invalid) {
        value = NULL;
    } else if (internal_state != Begin) {
        while (move_pointer_backward());
    }

    return *this;
}

} // url
} // oodles
