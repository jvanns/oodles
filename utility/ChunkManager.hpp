#ifndef OODLES_ALLOCATOR_CHUNKMANAGER_HPP
#define OODLES_ALLOCATOR_CHUNKMANAGER_HPP

// oodles
#include "Chunk.hpp"

// STL
#include <vector>

namespace oodles {
namespace allocator {

class ChunkManager
{
    public:
        /* Member functions/methods */
        ChunkManager();

        void* allocate();
        void deallocate(void *block);
        void initialise(size_t block_size, size_t page_size);

        // FIXME: Better method name
        size_t size() const { return block_size; }
    private:
        /* Member variables/attributes */
        size_t block_size;
        Chunk::byte_t blocks;
        std::vector<Chunk> chunks;
        Chunk *empty, *allocator, *deallocator;

        /* Member functions/methods */
        Chunk* find(void *block) const; // Locate chunk that owns block
};

} // allocator
} // oodles

#endif

