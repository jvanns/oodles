#ifndef OODLES_EVENT_PUBLISHER_HPP
#define OODLES_EVENT_PUBLISHER_HPP

// STL
#include <set>

namespace oodles {
namespace event {

class Subscriber; // Forward declaration for Publisher

struct Event
{
    /* Member functions/methods */
    Event();
    virtual ~Event() = 0;

    /*
     * Templated cast operator will automatically,
     * given the correct (related) type, cast this
     * base class object to your derived subclass.
     *
     * Publisher::event() will call this below...
     */
    template<typename Derived>
    operator const Derived& () const
    {
        return *static_cast<const Derived*>(this);
    }
};

class Publisher
{
    public:
        /* Member functions/methods */
        Publisher(Event &e);
        ~Publisher();

        void broadcast();
        bool add_subscriber(Subscriber &s);
        bool remove_subscriber(Subscriber &s);

        const Event& event() const { return *object; }
        size_t subscribed() const { return subscribers.size(); }
    private:
        /* Member variables/attributes */
        Event *object;
        std::set<Subscriber*> subscribers;
};

} // event
} // oodles

#endif

