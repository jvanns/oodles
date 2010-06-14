// oodles
#include "Chunk.hpp"

#include <assert.h> // For assert()

namespace oodles {
namespace allocator {

Chunk::Chunk() : mem(NULL), next_free_block(0), available_blocks(0)
{
}

/*
 * Reserve (allocate on the heap) enough space for a maximum
 * of p * q-sized blocks This makes up a whole chunk. I.e.
 * reserve_space(8, 256) will allocate enough space to hold
 * a 2048-byte size chunk of 256 8-byte sized blocks.
 */
void
Chunk::reserve_space(size_t block_size, byte_t blocks)
{
    assert(mem == NULL);
    
    assert(blocks > 0);
    assert(block_size > 0);

    mem = static_cast<byte_t*>(malloc(block_size * blocks));
    assert(mem != NULL);
    
    /* Initialise an index; set the 1st byte of every block to
     * the index/subscript value 1, 2, 3, ... n. Note that the
     * next_free_block is already 0 so we skip that one (I mean zero ;)
     */
    for (byte_t i = 1, *p = mem ; i != blocks ; p += block_size)
        *p = i++;

    allocated_blocks = available_blocks = blocks;
}

void
Chunk::destroy_space()
{
    assert(mem != NULL);

    free(static_cast<void*>(mem));
    mem = NULL;

    available_blocks = next_free_block = 0;
}

void*
Chunk::allocate(size_t block_size)
{
    if (full())
       return NULL;

    byte_t *offset = mem + (next_free_block * block_size);
    next_free_block = *offset;
    --available_blocks;

    return offset;
}

void
Chunk::deallocate(void *block, size_t block_size)
{
    byte_t *offset = static_cast<byte_t*>(block);
    const byte_t index = static_cast<byte_t>((offset - mem) / block_size);

    *offset = next_free_block;
    next_free_block = index;

    ++available_blocks;
}

} // allocator
} //oodles

