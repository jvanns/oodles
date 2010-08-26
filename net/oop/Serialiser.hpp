#ifndef OODLES_NET_OOP_SERIALISER_HPP
#define OODLES_NET_OOP_SERIALISER_HPP

// oodles
#include "Factory.hpp"
#include "Message.hpp"
#include "common/Exceptions.hpp"

// Boost.serialization
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// STL
#include <sstream>

namespace {

const std::ios_base::openmode BINARY_STREAM = std::ios_base::in | \
                                              std::ios_base::out | \
                                              std::ios_base::binary;

} // anonymous

namespace oodles {
namespace net {
namespace oop {

typedef boost::archive::text_iarchive Reconstructor; // Input archive stream
typedef boost::archive::text_oarchive Deconstructor; // Output archive stream

template<typename Type> class Serialiser : public Message, public Type
{
    public:
        /* Member functions/methods */
        Serialiser(uint32_t body_size = 0) :
            Message(id(), body_size),
            serialised(false),
            stream(BINARY_STREAM) {}
        
        id_t id() const
        {
           return Type::id();
        }
        
        Message* create(uint32_t body_size = 0) const
        {
            return new Serialiser<Type>(body_size);
        }

        /*
         * Use with Factory for concrete message type registration
         */
        static void init()
        {
            static Factory &f = Factory::instance();
            static const Serialiser<Type> me;

            f.enroll(&me);
        }

        void reconstruct() throw (ReadError)
        {
            if (serialised)
                return;
            
            bool success = true;

            try {
                Reconstructor r(stream, boost::archive::no_header);
                Type &me = *this;
                
                r >> me; // Start the Boost.serialize sequence
                success = !stream.fail();
            } catch (const std::exception &e) {
                success = false;
            }

            if (!success) {
                throw ReadError("Serialiser::reconstruct",
                                0,
                                "Message serialization error for type %d.",
                                id());
            }

            header.body_size = stream.tellg();
            serialised = true;
            reset_stream();
        }
        
        void deconstruct() throw (WriteError)
        {
            if (serialised)
                return;
            
            bool success = true;
            
            try {
                Deconstructor d(stream, boost::archive::no_header);
                const Type &me = *this;
                
                d << me; // Start the Boost.serialize sequence
                success = !stream.fail();
            } catch (const std::exception &e) {
                success = false;
            }
            
            if (!success) {
                throw WriteError("Serialiser::deconstruct",
                                 0,
                                 "Message serialization error for type %d.",
                                 id());
            }

            header.body_size = stream.tellp();
            serialised = true;
            reset_stream();
        }
        
        size_t to_buffer(char *buffer, size_t max)
        {
            size_t offset = 0, pending = header.body_size - stream.tellg();
            
            if (!header.processed && max >= header.header_size) {
                header2buffer(buffer, header);
                offset = header.header_size;
            }
            
            max -= offset;

            if (max > pending)
                max = pending;
            
            return offset + stream.rdbuf()->sgetn(buffer + offset, max);
        }

        size_t from_buffer(const char *buffer, size_t max)
        {
            size_t pending = header.body_size - stream.tellp();
            
            if (max > pending)
                max = pending;
            
            return stream.rdbuf()->sputn(buffer, max);
        }
    private:
        /* Member variables/attributes */
        bool serialised;
        std::stringstream stream;
        
        /* Member functions/methods */
        void reset_stream()
        {
            stream.seekg(0);
            stream.seekp(0);
            stream.clear();
        }
};

} // oop
} // net
} // oodles

#endif
