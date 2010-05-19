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

        /*
         * Prepare the index for the initial Domain/IP state
         */
        if (url->attributes.ip)
            index = 0;
        else
            index = url->attributes.domain.size() - 1;
    }

    switch (external_state) {
        case URL::Domain:
            if (url->attributes.domain.empty()) {
                external_state = URL::Path;
                recall = true;
                index = 0;
            } else if (index >= 0) {
                bool end = false;
                value = &url->attributes.domain[index];

                if (url->attributes.ip) {
                    if (index == url->attributes.domain.size() - 1)
                        end = true;
                    else
                        ++index;
                } else {
                    if (index == 0)
                        end = true;
                    else
                        --index;
                }

                if (end) {
                    external_state = URL::Path;
                    index = 0;
                }
            }
            break;
        case URL::Path:
            if (!url->attributes.path.empty() &&
                index < url->attributes.path.size()) {
                value = &url->attributes.path[index];
                ++index;
            } else {
                external_state = URL::Page;
                recall = true;
                index = 0;
            }
            break;
        case URL::Page:
            if (!url->attributes.page.empty())  // Ensure leaf node is non-empty
                value = &url->attributes.page;
            else
                recall = true;

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
        index = url->attributes.path.size() - 1; // Prepare index for Path state
    }

    switch (external_state) {
        case URL::Domain:
            if (!url->attributes.domain.empty() &&
                index < url->attributes.domain.size()) {
                value = &url->attributes.domain[index];

                if (url->attributes.ip) {
                    if (index == 0)
                        internal_state = End;
                    else
                        --index;
                } else
                    ++index;
            } else {
                external_state = -1; // Finish
                recall = true;
                index = 0;
            }
            break;
        case URL::Path:
            if (url->attributes.path.empty()) {
                external_state = URL::Domain;
                recall =  true;
            } else if (index >= 0) {
                value = &url->attributes.path[index];

                if (index == 0) {
                    external_state = URL::Domain;

                    if (url->attributes.ip)
                        index = url->attributes.domain.size() - 1;
                } else
                    --index;
            }
            break;
        case URL::Page:
            if (!url->attributes.page.empty())
                value = &url->attributes.page;
            else
                recall = true;

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
