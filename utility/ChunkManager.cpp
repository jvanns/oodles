// oodles
#include "ChunkManager.hpp"

// STL
#ifdef DEBUG_ALLOCATOR
#include <iostream>
#endif

#include <assert.h> // For assert()

using std::swap;
using std::vector;

namespace oodles {
namespace allocator {

ChunkManager::ChunkManager() :
    block_size(0),
    blocks(0),
    empty(NULL),
    allocator(NULL),
    deallocator(NULL)

{
}

ChunkManager::~ChunkManager()
{
    vector<Chunk>::iterator i = chunks.begin(), j = chunks.end();

    while (i != j) {
        i->destroy_space();
        ++i;
    }
}

void*
ChunkManager::allocate()
{
    if (!allocator || allocator->full()) {
        bool acquired = false;
        vector<Chunk>::iterator i = chunks.begin(), j = chunks.end();
        
        if (empty != NULL) {
            allocator = empty;
            acquired = true;
            empty = NULL;
        }

        while (!acquired) {
            if (i == j) { // If we happen to have no chunks left...
                const size_t size = chunks.size();

                /*
                 * When needing more space in chunks grow it
                 * by a factor of n to avoid having to do it 
                 * all over again too often in the near future.
                 */
                if (size == chunks.capacity()) {
                    const size_t growth = size == 0 ? 2 : size * 2;
                    chunks.reserve(growth); // Double our available capacity

#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: ChunkManager "
              << this << " available capacity is "
              << growth << " Chunks.\n";
#endif
                }

                chunks.resize(size + 1); // Will create/append a new Chunk()

#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: ChunkManager "
              << this << " size is "
              << size + 1 << " Chunks.\n";
#endif

                allocator = &chunks.back();
                deallocator = &chunks.front();
                allocator->reserve_space(block_size, blocks);

                acquired = true;
            } else if (!i->full()) { // Has this chunk got any blocks available?
                allocator = &(*i);
                acquired = true;
            }

            ++i;
        }
    } else if (allocator == empty) {
        /* When allocator and empty point to the same Chunk we must
         * be sure to set empty to NULL because we're about to call
         * allocate() on it again thus rendering it non-empty again!
         */
        empty = NULL;
    }

    assert(allocator != NULL);
    assert(!allocator->full());

    return allocator->allocate(block_size);
}

void
ChunkManager::deallocate(void *block)
{
    assert(!chunks.empty());
    assert(deallocator != NULL);
    
    Chunk *owner = find(block);

    assert(owner != NULL);
    assert(!owner->empty());

#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: Chunk " << owner
              << " owns block " << block << ".\n";
#endif

    /*
     * The probability that the next deallocate()
     * request will be to the newly found chunk.
     * So, as a hint to the next find() we assign
     * ...
     */
    deallocator = owner; // ... owner to deallocator!
    deallocator->deallocate(block, block_size);

    if (deallocator->empty()) { // Is this chunk now unused?
        assert(deallocator != empty); // We cannot empty the same chunk twice!

        if (empty != NULL) { // Is there an existing empty chunk?
            Chunk *end = &chunks.back();

            if (deallocator == end) // Is the deallocator chunk the end chunk?
                deallocator = empty; // Then point it to the empty one
            else if (empty != end)
                swap(*empty, *end);

            assert(end->empty());

            end->destroy_space();
            chunks.pop_back();

            if (allocator == end || allocator->full())
                allocator = deallocator; // Re-use this empty chunk
        }
        
        empty = deallocator;
    }

    assert(empty == NULL || empty->empty());
}

void
ChunkManager::initialise(size_t block_size, size_t page_size)
{
    assert(page_size > 0);
    assert(block_size > 0);
    assert(chunks.empty());

    this->block_size = block_size;
    blocks = page_size / block_size;

    if (blocks > maximum_chunk_size())
        blocks = maximum_chunk_size();
    else if (blocks < minimum_chunk_size())
        blocks = minimum_chunk_size();
}

inline
Chunk*
ChunkManager::find(void *block) const
{
    bool done = false;
    Chunk *l = deallocator, *u = l + 1;
    const size_t chunk_size = blocks * block_size;
    const Chunk *lb = &chunks.front(), *ub = &chunks.back();

    /*
     * The constants above mark the Lower and Upper Bounds
     * of the available chunks we manage. Both l and b below
     * are the current lower and upper pointers we search.
     */
    while (!done) {
        if (l) { // Remember; l == lower
            if (l->owns_block(block, chunk_size))
                return l;

            if (l == lb)
                l = NULL; // Lower chunks exhausted
            else
                --l; // Next, try the preceeding chunk
        }

        if (u) { // Remember; u = upper
            if (u->owns_block(block, chunk_size))
                return u;

            if (u == ub)
                u = NULL; // Upper chunks exhausted
            else
                ++u; // Next, try the successive chunk
        }

        done = !l && !u;
    }

    return NULL;
}

} // allocator
} // oodles
