// oodles
#include "URL.hpp"
#include "Iterator.hpp"

namespace oodles {
namespace url {

Iterator::Iterator(const URL *target, int state) :
    internal_state(state),
    external_state(-1),
    value(NULL),
    url(target),
    index(0)
{
    move_pointer_forward();
}

Iterator::Iterator(const Iterator &rhs)
{
    if (this != &rhs)
        *this = rhs;
}

Iterator&
Iterator::operator= (const Iterator &rhs)
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
Iterator::move_pointer_forward()
{
    bool recall = false;

    if (internal_state == Begin) {
        internal_state = Transient;
        external_state = URL::Scheme;
    }

    switch (external_state) {
        case URL::Scheme:
            value = &url->attributes.scheme;
            external_state = URL::Username;
            break;
        case URL::Username:
            value = &url->attributes.username;
            external_state = URL::Password;
            break;
        case URL::Password:
            value = &url->attributes.password;
            external_state = URL::Domain;
            break;
        case URL::Domain:
            if (!url->attributes.domain.empty() &&
                index < url->attributes.domain.size()) {
                value = &url->attributes.domain[index];
                ++index;
            } else {
                external_state = URL::Port;
                recall = true;
                index = 0;
            }
            break;
        case URL::Port:
            value = &url->attributes.port;
            external_state = URL::Path;
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
            if (!url->attributes.page.empty())
                value = &url->attributes.page;
            else
                recall = true;

            external_state = -1;
            break;
        default:
            value = NULL;
            internal_state = End;
            break;
    }

    return recall;
}

bool
Iterator::move_pointer_backward()
{
    bool recall = false;

    if (internal_state == End) {
        internal_state = Transient;
        external_state = URL::Page;
        index = url->attributes.path.size() - 1; // Prepare index for Path state
    }

    switch (external_state) {
        case URL::Scheme:
            value = &url->attributes.scheme;
            external_state = -1;
            break;
        case URL::Username:
            value = &url->attributes.username;
            external_state = URL::Scheme;
            break;
        case URL::Password:
            value = &url->attributes.password;
            external_state = URL::Username;
            break;
        case URL::Domain:
            if (url->attributes.domain.empty()) {
                external_state = URL::Password;
                recall = true;
            } else if (index >= 0) {
                value = &url->attributes.domain[index];

                if (index == 0)
                    external_state = URL::Password;
                else
                    --index;
            }
            break;
        case URL::Port:
            value = &url->attributes.port;
            external_state = URL::Domain;
            index = url->attributes.domain.size() - 1; // Prepare for next state
            break;
        case URL::Path:
            if (url->attributes.path.empty()) {
                external_state = URL::Port;
                recall = true;
            } else if (index >= 0) {
                value = &url->attributes.path[index];

                if (index == 0)
                    external_state = URL::Port;
                else
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

Iterator&
Iterator::operator++()
{
    if (internal_state == Invalid) {
        value = NULL;
    } else if (internal_state != End) {
        while (move_pointer_forward());
    }

    return *this;
}

Iterator&
Iterator::operator--()
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
