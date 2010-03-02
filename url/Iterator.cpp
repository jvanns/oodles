// oodles
#include "Iterator.hpp"

namespace oodles {

Iterator::Iterator(const URL *u, State s) :
    value(NULL),
    url(u),
    index(0),
    state(s)
{
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
        state = rhs.state;
    }

    return *this;
}

//   bool position_pointer(Direction d)
//    {
//        bool move = false;
//
//        if (state == End && d == Backward) {
//            state = Transient;
//            --index;
//        }
//
//        switch (spread) {
//            case Container::S1:
//                if (container->c1.size() > index) { // c1 isn't exhausted yet
//                    element = &container->c1.at(index);
//                    move = true;
//                } else {
//                    spread = Container::S2; // state transition
//                    index = 0; // Must reset index!
//                }
//                break;
//            case Container::S2:
//                if (container->c2.size() > index) { // c2 isn't exhausted yet
//                    element = &container->c2.at(index);
//                    move = true;
//                } else {
//                    element = NULL; // Element is invalid
//                    state = End; // ... is nigh ;_)
//                }
//                break;
//        }
//
//        if (move) {
//            if (d == Forward)
//                index++;
//            else
//                index--;
//        }
//
//        return move;
//    }

Iterator&
Iterator::operator++()
{
    if (state == Invalid) {
        value = NULL;
    } else if (state != End) {
    }

    return *this;
}

Iterator&
Iterator::operator--()
{
    if (state == Invalid) {
        value = NULL;
    } else if (state != Begin) {
    }

    return *this;
}

} // oodles
