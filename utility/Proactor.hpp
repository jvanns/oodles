#ifndef OODLES_PROACTOR_HPP
#define OODLES_PROACTOR_HPP

// Boost.asio
#include <boost/asio/io_service.hpp>

// Boost.threads
#include <boost/thread/thread.hpp>

namespace oodles {

class Proactor
{
    public:
        /* Member variables/attributes */
        Proactor(size_t threads = boost::thread::hardware_concurrency());
        ~Proactor();

        boost::asio::io_service& io_service() { return ios; }
    private:
        /* Member variables/attributes */
        boost::thread_group threads;
        boost::asio::io_service ios;
        boost::asio::io_service::work work;
};
   
} // oodles

#endif

