#ifndef OODLES_MATH_HPP
#define OODLES_MATH_HPP

namespace oodles {

template<class T>
double normalise(const T &n, // Arbitrary value
                 const T &to_min, // Target range
                 const T &to_max,
                 const T &from_min, // Source range
                 const T &from_max);

} // oodles

#include "math.ipp" // Implementation

#endif

