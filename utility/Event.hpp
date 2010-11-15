#ifndef OODLES_EVENT_EVENT_HPP
#define OODLES_EVENT_EVENT_HPP

// STL
#include <set>

// Boost.shared_ptr
#include <boost/shared_ptr.hpp>

namespace oodles {
namespace event {

class Publisher; // Forward declaration for Event
class Subscriber; // Forward declaration for Event

class Event
{
    public:
        /* Dependent typedefs */
        typedef boost::shared_ptr<Event> Ref;
        
        /* Member functions/methods */
        Event();
        virtual ~Event();

        virtual Event* clone() const = 0;
        bool add_subscriber(Subscriber &s);
        bool remove_subscriber(Subscriber &s);
        void publish(const Publisher &p) const;

        /*
         * Templated cast operator will automatically,
         * given the correct (related) type, cast this
         * base class object to your derived subclass.
         */
        template<typename Derived>
        operator const Derived& () const
        {
            return *static_cast<const Derived*>(this);
        }
    private:
        /* Member variables/attributes */
        std::set<Subscriber*> subscribers;

        /* Friend class declarations */
        friend class Publisher; // Accesses subscribers
        friend class Subscriber; // Accesses subscribers
};

} // event
} // oodles

#endif

