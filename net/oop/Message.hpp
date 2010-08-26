#ifndef OODLES_NET_OOP_MESSAGE_HPP
#define OODLES_NET_OOP_MESSAGE_HPP

// oodles
#include "id_t.hpp"
#include "common/Exceptions.hpp"

namespace oodles {
namespace net {
namespace oop {

class Message
{
    public:
        /* Dependent typedefs */
        struct Header {
            static const uint8_t header_size;
            uint32_t body_size;
            id_t message_id;
            bool processed;

            Header() : body_size(0), message_id(INVALID_ID), processed(false) {}
        };

        /* Member functions/methods */
        Message(id_t id = INVALID_ID, uint32_t body_size = 0);
        virtual ~Message();
        
        static Header buffer2header(const char *buffer);
        static void header2buffer(char *buffer, Header &h);
        
        /*
         * Cast operators for casting up the class hierarchy to
         * our concrete, derived messages.
         */
        template<typename Type>
        operator Type&() { return static_cast<Type&>(*this); }
        
        virtual id_t id() const = 0;
        virtual Message* create(uint32_t body_size = 0) const = 0;
        
        virtual void reconstruct() throw(ReadError) = 0;
        virtual void deconstruct() throw(WriteError) = 0;
        
        virtual size_t to_buffer(char *buffer, size_t max) = 0;
        virtual size_t from_buffer(const char *buffer, size_t max) = 0;
        
        size_t size() const { return header.header_size + header.body_size; }
    protected:
        /* Member variables/attributes */
        Header header;
};

} // oop
} // net
} // oodles

#endif

