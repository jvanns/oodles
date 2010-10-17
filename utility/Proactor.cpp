// oodles
#include "Proactor.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost
using boost::bind;
using boost::asio::io_service;

namespace oodles {

Proactor::Proactor(size_t threads) : running(false), work(ios)
{
    if (!threads)
        threads = 1;

    for (size_t i = 0 ; i < threads ; ++i)
        this->threads.create_thread(bind(&io_service::run, &ios));

    running = true;
}

Proactor::~Proactor()
{
    stop();
    wait();
}

void
Proactor::stop()
{
    if (running) {
        ios.stop();
        running = false;
    }
}

void
Proactor::wait()
{
    threads.join_all();
}
 
} // oodles

