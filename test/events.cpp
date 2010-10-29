// oodles
#include "utility/Event.hpp"
#include "utility/Proactor.hpp"
#include "utility/Publisher.hpp"
#include "utility/Subscriber.hpp"

// STL
#include <vector>
#include <sstream>
#include <iostream>

// libc
#include <math.h> // For sqrt()
#include <assert.h> // For assert()

// STL
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::ostringstream;

struct Integers : public oodles::event::Event
{
    vector<int> v;
};

struct Sum : public oodles::event::Subscriber
{
    void receive(const oodles::event::Event &e)
    {
        const Integers &m = e;
        cout << "Sum: " << calculate_sum(m) << std::endl;
    };

    static unsigned long long calculate_sum(const Integers &m)
    {
        vector<int>::const_iterator i = m.v.begin(), j = m.v.end();
        unsigned long long sum = 0;

        while (i != j) {
            sum += *i;
            ++i;
        }

        return sum;
    }
};

struct Deviation : public oodles::event::Subscriber
{
    void receive(const oodles::event::Event &e)
    {
        const Integers &m = e;
        cout << "Standard deviation: " << calculate_deviation(m) << std::endl;
    };

    static float calculate_deviation(const Integers &m)
    {
        vector<int>::const_iterator i = m.v.begin(), j = m.v.end();
        float sum = Sum::calculate_sum(m), mean = sum / m.v.size();

        for (sum = 0.0f ; i != j ; ++i)
            sum += (*i - mean) * (*i - mean); // square the difference

        return sqrt(sum / m.v.size());
    }
};

struct Concatenate : public oodles::event::Subscriber
{
    void receive(const oodles::event::Event &e)
    {
        const Integers &m = e;
        cout << "Concatenation: " << concatenate_integers(m) << std::endl;
    };

    static string concatenate_integers(const Integers &m)
    {
        vector<int>::const_iterator i = m.v.begin(), j = m.v.end();
        ostringstream ss;
        string s;

        for ( ; i != j ; ++i, ss.str("")) {
            ss << *i;
            s += ss.str();
        }

        return s;
    }
};

int main(int argc, char *argv[])
{
    Integers e; // Event object
    oodles::Proactor t; // Task dispatcher
    oodles::event::Publisher p(&t); // Publisher

    Sum s; // Subscriber
    Deviation d; // Another, different Subscriber
    Concatenate c; // Yet another, different Subscriber ;)

    if (!e.add_subscriber(s)) {
        cerr << "Sum failed to subscribe to publisher!" << endl;
        return 1;
    }

    if (!e.add_subscriber(d)) {
        cerr << "Deviation failed to subscribe to publisher!" << endl;
        return 1;
    }

    if (!e.add_subscriber(c)) {
        cerr << "Concatenate failed to subscribe to publisher!" << endl;
        return 1;
    }

    assert(s.subscribed_to() == 1);
    assert(d.subscribed_to() == 1);
    assert(c.subscribed_to() == 1);

    static const int size = (1024 * 1024) / sizeof(int);
    e.v.reserve(size);

    /*
     * Do something with our event object - just
     * populating it with some integers will do!
     */
    for (int n = 1 ; n <= size ; ++n)
        e.v.push_back(n);

    e.publish(p);
    t.wait();

    return 0;
}
