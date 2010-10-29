#ifndef OODLES_EVENT_PUBLISHER_HPP
#define OODLES_EVENT_PUBLISHER_HPP

// libc
#include <stdlib.h> // For NULL

namespace oodles {

class Proactor; // Forward declaration for Publisher

namespace event {

class Event; // Forward declaration for Publisher

class Publisher
{
    public:
        /* Member functions/methods */
        Publisher(Proactor *p = NULL);
        void broadcast(const Event &e) const;
    private:
        /* Member variables/attributes */
        Proactor *proactor;
};

} // event
} // oodles

#endif

