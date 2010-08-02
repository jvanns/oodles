// oodles
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
    void receive(const oodles::event::Publisher &p)
    {
        const Integers &m = p.event();
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
    void receive(const oodles::event::Publisher &p)
    {
        const Integers &m = p.event();
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
    void receive(const oodles::event::Publisher &p)
    {
        const Integers &m = p.event();
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
    oodles::event::Publisher::Proactor t;
    oodles::event::Publisher p(e, &t); // Publisher

    Sum s; // Subscriber
    Deviation d; // Another, different Subscriber
    Concatenate c; // Yet another, different Subscriber ;)

    if (!p.add_subscriber(s)) {
        cerr << "Sum failed to subscribe to publisher!" << endl;
        return 1;
    }

    if (!p.add_subscriber(d)) {
        cerr << "Deviation failed to subscribe to publisher!" << endl;
        return 1;
    }

    if (!p.add_subscriber(c)) {
        cerr << "Concatenate failed to subscribe to publisher!" << endl;
        return 1;
    }

    assert(s.subscribed_to(p));
    assert(d.subscribed_to(p));
    assert(c.subscribed_to(p));
    assert(p.subscribed() == 3);

    static const int size = (1024 * 1024) / sizeof(int);
    e.v.reserve(size);

    /*
     * Do something with our event object - just
     * populating it with some integers will do!
     */
    for (int n = 1 ; n <= size ; ++n)
        e.v.push_back(n);

    if (p.broadcast())
        t.run(); // Ideally, this should be executed in a thread

    return 0;
}
