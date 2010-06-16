#ifndef OODLES_ALLOCATOR_OBJECTALLOCATOR_HPP
#define OODLES_ALLOCATOR_OBJECTALLOCATOR_HPP

// oodles
#include "ChunkManager.hpp"

namespace oodles {
namespace allocator {

class ObjectAllocator
{
    public:
        /* Member functions/methods */
        void* allocate(size_t bytes);
        void deallocate(void *block, size_t bytes);

        static ObjectAllocator& instance();
    private:
        /* Member functions/methods */
        ObjectAllocator();
        ~ObjectAllocator();

        /* Member variables/attributes */
        ChunkManager *allocators;
        ChunkManager *allocator, *deallocator;
};

} // allocator
} // oodles

#endif

