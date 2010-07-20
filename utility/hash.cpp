// oodles
#include "hash.hpp"

namespace oodles {

template<typename Integer> class FNV
{
    public:
        FNV(const Integer p, const Integer o) : prime(p), offset(o) {}

        Integer operator() (const char *input, size_t size, Integer seed) const
        {
            if (!seed)
                seed = offset; // If no seed is given, start with offset

            for (size_t i = 0 ; i < size ; ++i)
                seed = (seed * prime) ^ input[i];

            return seed;
        }
    private:
        const Integer prime, offset;
};

/*
 * Common hashing method (FNV) to a 64-bit integer.
 * Refer to the following for algorithm details:
 */
uint64_t
fnv64(const char *input, size_t size, uint64_t seed)
{
    FNV<uint64_t> f(1099511628211U, 14695981039346656037U);
    return f(input, size, seed);
}

/*
 * Common hashing method (FNV) to a 32-bit integer.
 * Refer to the following for algorithm details:
 */
uint32_t
fnv32(const char *input, size_t size, uint32_t seed)
{
    FNV<uint32_t> f(16777619U, 2166136261U);
    return f(input, size, seed);
}

} // oodles

