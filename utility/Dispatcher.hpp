#ifndef OODLES_DISPATCHER_HPP
#define OODLES_DISPATCHER_HPP

// Boost.asio
#include <boost/asio/io_service.hpp>

// Boost.threads
#include <boost/thread/thread.hpp>

namespace oodles {

class Dispatcher
{
    public:
        /* Member variables/attributes */
        Dispatcher(size_t threads = boost::thread::hardware_concurrency());
        ~Dispatcher();

        void stop();
        void wait();
        bool stopped() const { return !running; }
        boost::asio::io_service& io_service() { return ios; }
    private:
        /* Member variables/attributes */
        bool running;
        boost::thread_group threads;
        boost::asio::io_service ios;
        boost::asio::io_service::work work;
};
   
} // oodles

#endif

