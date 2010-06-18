// oodles
#include "utility/SmallObject.hpp"

// STL
#include <vector>
#include <string>

struct A : public oodles::allocator::SmallObject
{
    int b;
    float c;

    virtual ~A() {}
};

struct B : public A
{
    char b;
    unsigned long long c;
};

struct C : public oodles::allocator::SmallObject
{
    void *b;
};

struct D : public oodles::allocator::SmallObject
{
    double b;
    char c[100];
};

struct E : public oodles::allocator::SmallObject
{
    struct b {
        const int c;

        b(int x) : c(x) {}
    };

    static const b d, *e;
};

const E::b E::d(42);
const E::b* E::e = &E::d;

struct V : public B
{
    std::vector<std::string> v;
};

struct W : public B
{
    char a[512];
};

struct X
{
    bool b;
    char c[3];
};

int main(int argc, char *argv[])
{
    A *a = new A;
    B *b = new B;
    C *c = new C;
    D *d = new D;
    E *e = new E;
    X *x = new X;

    delete a;
    delete b;
    delete c;
    delete d;
    delete e;
    delete x;

    for (int i = 0 ; i < CHAR_MAX ; ++i) {
        a = new B;
        b = static_cast<B*> (a);

        delete b;
        a = b = NULL;
    }

    A **f = static_cast<A**> (malloc(USHRT_MAX * sizeof(A)));

    for (int i = 0 ; i < USHRT_MAX ; ++i) {
        if (i % 17 == 0)
            f[i] = new V;
        else if (i % 22 == 0)
            f[i] = new W;
        else
            f[i] = new B;
    }

    for (int i = 0 ; i < USHRT_MAX ; ++i)
        delete f[i];

    free(f);

    return 0;
}
