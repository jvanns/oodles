#ifndef OODLES_MATH_IPP // Implementation
#define OODLES_MATH_IPP

namespace oodles {

template<class T>
double
normalise(const T &n, // Arbitrary value
          const T &to_min, // Target range
          const T &to_max,
          const T &from_min, // Source range
          const T &from_max)
{
    return (n / ((from_max - from_min) * (to_max - to_min))) + to_min;
}

} // oodles

#endif

