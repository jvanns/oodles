#ifndef OODLES_HASH_HPP
#define OODLES_HASH_HPP

#include <stdint.h> // For uint64_t
#include <stdlib.h> // For size_t

namespace oodles {

uint64_t fnv64(const char *input, size_t size, uint64_t seed = 0);

} // oodles

#endif
