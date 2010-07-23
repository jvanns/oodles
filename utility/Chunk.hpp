#ifndef OODLES_ALLOCATOR_CHUNK_HPP
#define OODLES_ALLOCATOR_CHUNK_HPP

#include <stdlib.h> // For size_t
#include <stdint.h> // For uint16_t
#include <limits.h> // For CHAR_BIT etc.

namespace oodles {
namespace allocator {

/*
 * All allocator related code is an implementation of the Small Object
 * Allocator work described in Andrei Alexandrescu's book Modern C++
 * Design. This implementation is simple and builds only on the text in
 * the book so may not be as full-featured as the solution found in Loki.
 */

struct Chunk
{
    /* Dependent typedefs */
    typedef unsigned char byte_t;

    /* Member functions/methods */
    Chunk();

    /*
     * Using the 'stealth indices' we are limited by the
     * number of blocks we can allocate rather than their
     * individual size.
     */
    void reserve_space(size_t block_size, byte_t blocks);
    void destroy_space();

    void* allocate(size_t block_size);
    void deallocate(void *block, size_t block_size);

    /*
     * Does the address of 'block' fall within
     * the range allocated to 'mem'?
     */
    bool owns_block(void *block, size_t chunk_size) const
    {
        if (empty())
            return false;

        const byte_t *p = static_cast<byte_t*>(block);
        return (p >= mem) && (p < (mem + chunk_size));
    }

    bool full() const { return available_blocks == 0; }
    bool empty() const { return !mem || available_blocks == allocated_blocks; };

    /* Member variables/attributes */
    byte_t *mem, next_free_block, available_blocks, allocated_blocks;
};

inline uint16_t alignment()
{
#ifdef __GNUC__
    return __alignof__(Chunk::byte_t*);
#endif
    return sizeof(Chunk::byte_t*);
}

inline Chunk::byte_t minimum_chunk_size()
{
    return CHAR_BIT;
}

inline Chunk::byte_t maximum_chunk_size()
{
    return UCHAR_MAX;
}

} // allocator
} // oodles

#endif

