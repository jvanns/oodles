// oodles
#include "hash.hpp"

namespace oodles {

/*
 * Common hashing method (FNV) to a 64-bit integer.
 * Refer to the following for algorithm details:
 */
uint64_t
fnv64(const char *input, size_t size, uint64_t seed)
{
    static const uint64_t prime = 1099511628211U,
                          offset = 14695981039346656037U;

    if (!seed)
        seed = offset; // If no seed (previous hash) is given, start with offset

    for (size_t i = 0 ; i < size ; ++i)
        seed = (seed * prime) ^ input[i];

    return seed;
}

} // oodles

