#ifndef OODLES_HASH_HPP
#define OODLES_HASH_HPP

#include <stdint.h> // For uint64_t
#include <stdlib.h> // For size_t

#ifdef __GNUG__
#ifdef __LP64__
#define HAS_64_BITS
#endif
#endif

namespace oodles {

uint32_t fnv32(const char *input, size_t size, uint32_t seed = 0);

#ifdef HAS_64_BITS
uint64_t fnv64(const char *input, size_t size, uint64_t seed = 0);
#endif

} // oodles

#endif
