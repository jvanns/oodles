#ifndef OODLES_NET_BUFFER_HPP
#define OODLES_NET_BUFFER_HPP

// Boost.asio
#include <boost/asio/streambuf.hpp>

namespace oodles {
namespace net {

template<size_t max> class Buffer
{
    private:
        class Producer; // Forward declaraton for Buffer
        class Consumer; // Forward declaraton for Buffer
    public:
        /* Member functions/methods */
        Buffer() : p(buffer), c(buffer), buffer(max) {}

        Producer& producer() { return p; }
        Consumer& consumer() { return c; }
    private:
        /* Internal Data Structures */
        class Producer
        {
            public:
                /* Member functions/methods */
                Producer(boost::asio::streambuf &b) : buffer(b) {}

                /*
                 * Prepares the output sequence ready for writing
                 * data into the buffer.
                 */
                size_t prepare_buffer(char *&ptr)
                {
                    size_t u = buffer.size(), m = buffer.max_size(), a = m - u;

                    if (u == m)
                        return 0;

                    ptr = boost::asio::buffer_cast<char*>(buffer.prepare(a));

                    return a;
                }

                /*
                 * Commits the prepared output sequence of the
                 * buffer to the input sequence ready for reading
                 */
                size_t commit_buffer(size_t bytes)
                {
                    buffer.commit(bytes);
                    return buffer.size();
                }
            private:
                /* Member variables/attributes */
                boost::asio::streambuf &buffer;
        };

        class Consumer
        {
            public:
                /* Member functions/methods */
                Consumer(boost::asio::streambuf &b) : buffer(b) {}

                /*
                 * Returns the input sequence of the buffer for reading
                 */
                const char* data() const
                {
                    return boost::asio::buffer_cast<const char*>(buffer.data());
                }
                
                /*
                 * Consumes data from the input sequence of the buffer
                 * effectively removing and creating space for new data
                 */
                size_t consume_buffer(size_t bytes)
                {
                    buffer.consume(bytes);
                    return buffer.size();
                }
            private:
                /* Member variables/attributes */
                boost::asio::streambuf &buffer;
        };

        /* Member variables/attributes */
        Producer p;
        Consumer c;
        boost::asio::streambuf buffer; // The internal fixed-size byte buffer
};

} // net
} // oodles

#endif

