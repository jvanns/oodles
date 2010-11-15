#ifndef OODLES_EVENT_SUBSCRIBER_HPP
#define OODLES_EVENT_SUBSCRIBER_HPP

// oodles
#include "Event.hpp"

// STL
#include <set>
#include <map>

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
         * Override this method to receive events dispatched from the publisher.
         */
        virtual void receive(const Event::Ref e) = 0;
        size_t subscribed_to() const { return events.size(); }
    private:
        /* Member functions/methods */
        bool subscribe_to(Event &e);
        bool unsubscribe_from(Event &e);

         /* Dependent typedefs */
        typedef std::set<Subscriber*>::iterator Locator;

        /* Member variables/attributes */
        std::map<Event*, Locator> events;

        /* Friend class declarations */
        friend class Event; // Calls the two private methods above
};

} // event
} // oodles

#endif

