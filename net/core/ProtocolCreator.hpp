#ifndef OODLES_NET_PROTOCOLCREATOR_HPP
#define OODLES_NET_PROTOCOLCREATOR_HPP

// STL
#include <typeinfo>

// libc
#include <stdio.h> // For NULL

namespace oodles {
namespace net {

struct ProtocolHandler; // Forward declaration for ProtocolCreator

struct ProtocolDialect
{
    /* Member functions/methods */
    ProtocolDialect() : handler(NULL) {}
    virtual ~ProtocolDialect() {}

    void set_handler(ProtocolHandler &p) { handler = &p; }

    /*
     * Cast operators for casting up the class hierarchy to
     * our concrete, derived subclass.
     */
    template<typename Dialect>
    operator Dialect&() { return static_cast<Dialect&>(*this); }

    /* Member variables/attributes */
    ProtocolHandler *handler; // The link to our protocol handler
};

struct NoDialect : public ProtocolDialect
{
};

class DialectCreator
{
    public:
        /* Member functions/methods */
        DialectCreator() {}
        virtual ~DialectCreator() {}
        virtual ProtocolDialect* create(ProtocolHandler &h) const = 0;
    private:
        /* Member functions/methods */
        DialectCreator(const DialectCreator &p);
        DialectCreator& operator= (const DialectCreator &p);
};

template<typename Dialect> struct ProtocolInterpreter : public DialectCreator
{
    public:
        /* Member functions/methods */
        ProtocolInterpreter() :
            no_dialect(typeid(NoDialect)),
            chosen_dialect(typeid(Dialect))
        {
        }
        
        ProtocolDialect* create(ProtocolHandler &h) const
        {
            if (chosen_dialect == no_dialect)
                return &h;
            
            return new Dialect;
        }
    private:
        /* Member variables/attributes */
        const std::type_info &no_dialect, &chosen_dialect;
};

class ProtocolCreator
{
    public:
        /* Member functions/methods */
        ProtocolCreator() {}
        virtual ~ProtocolCreator() {}
        virtual ProtocolHandler* create() const = 0;
    private:
        /* Member functions/methods */
        ProtocolCreator(const ProtocolCreator &p);
        ProtocolCreator& operator= (const ProtocolCreator &p);
};

template<typename Handler, typename Dialect = NoDialect>
class Protocol : public ProtocolCreator
{
    public:
        /* Member functions/methods */
        Protocol() {}
        ProtocolHandler* create() const { return new Handler(interpreter); }
    private:
        /* Member variables/attributes */
        const ProtocolInterpreter<Dialect> interpreter;
};

} // net
} // oodles

#endif
