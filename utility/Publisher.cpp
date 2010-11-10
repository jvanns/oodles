// oodles
#include "Event.hpp"
#include "Proactor.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost
using boost::bind;

// STL
using std::set;

namespace {

class Ref
{
    public:
        /* Member functions/methods */
        inline explicit Ref(const oodles::event::Event &e) : ref(&e) {}
        inline Ref(const Ref &r) : ref(r.ref) {}
        
        inline operator const oodles::event::Event& () const { return *ref; }
    private:
        /* Member variables/attributes */
        const oodles::event::Event* const ref;

        /* Member functions/methods */
        Ref& operator= (const Ref &r);
};
 
} // anonymous

namespace oodles {
namespace event {

Publisher::Publisher(Proactor *p) : proactor(p)
{
}

void
Publisher::broadcast(const Event &e) const
{
    const Ref ref(e);
    set<Subscriber*>::iterator i = e.subscribers.begin(),
                               j = e.subscribers.end();

    while (i != j) {
        Subscriber *s = *i;

        if (proactor)
            proactor->io_service().post(bind(&Subscriber::receive, s, ref));
        else
            s->receive(ref); // Will block

        ++i;
    }
}

} // event
} // oodles

