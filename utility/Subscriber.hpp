#ifndef OODLES_EVENT_SUBSCRIBER_HPP
#define OODLES_EVENT_SUBSCRIBER_HPP

// libc
#include <stdlib.h> // For NULL

namespace oodles {
namespace event {

class Publisher; // Forward declaration for Subscriber

class Subscriber
{
    public:
        /* Member functions/methods */
        Subscriber();
        virtual ~Subscriber();

        /*
         * Override this method to receive events
         * from the publisher by calling p.event().
         */
        virtual void receive(const Publisher &p) = 0;
        bool subscribed() const { return publisher != NULL; }
        bool subscribed_to(Publisher &p) const { return publisher == &p; }
    private:
        /* Member variables/attributes */
        Publisher *publisher;

        bool subscribe_to(Publisher &p);
        bool unsubscribe_from(Publisher &p);
        void clear_publisher() { publisher = NULL; }

        /* Friend class declarations */
        friend class Publisher; // Publisher (only) can call subscribe_*()
};

} // event
} // oodles

#endif

