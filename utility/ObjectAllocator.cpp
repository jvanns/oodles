// oodles
#include "ObjectAllocator.hpp"

// STL
#ifdef DEBUG_ALLOCATOR
#include <iostream>
#endif

#include <assert.h> // For assert()
#include <unistd.h> // For sysconf()

/*
 * You should only specify PAGE_SIZE to the
 * pre-processor for testing - rely on either
 * your OS to determine it for you or use the
 * hard-coded oodles default of 4k
 */
static inline unsigned long page_size()
{
    static const unsigned long x =
#ifdef PAGE_SIZE
    PAGE_SIZE;
#else
    #ifndef WINDOWS
        sysconf(_SC_PAGESIZE);
    #else
        4096;
    #endif
#endif
    return x;
}

static inline void* native_alloc(size_t bytes)
{
#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: Native malloc() "
              << "services allocations for "
              << bytes << "b objects.\n";
#endif

    return malloc(bytes);
}

static inline void native_dealloc(void *block, size_t bytes)
{
#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: Native free() "
              << "services deallocations for "
              << bytes << "b objects.\n";
#endif

    free(block);
}

namespace oodles {
namespace allocator {

ObjectAllocator::ObjectAllocator() :
    allocators(NULL),
    allocator(NULL),
    deallocator(NULL)
{
    size_t managers = (maximum_chunk_size() + (alignment() - 1)) / alignment();

    allocators = new ChunkManager[managers];

    for (size_t i = 0 ; i < managers ; ++i)
        allocators[i].initialise((i + 1) * alignment(), page_size());
}

ObjectAllocator::~ObjectAllocator()
{
    delete[] allocators;
}

void*
ObjectAllocator::allocate(size_t bytes)
{
#ifdef OODLES_ALLOCATOR
    if (bytes > maximum_chunk_size())
#endif
        return native_alloc(bytes); // Revert to the C standard allocator

    /*
     * n byte-sized objects of this word alignment will
     * fit into the same fixed-sized allocator
     */
    const size_t index = ((bytes + (alignment() - 1)) / alignment()) - 1;
    allocator = &allocators[index];

    assert(allocator->size() == bytes);

#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: ChunkManager "
              << allocator << '/' << index
              << " services allocations for "
              << bytes << "b objects.\n";
#endif

    return allocator->allocate();
}

void
ObjectAllocator::deallocate(void *block, size_t bytes)
{
#ifdef OODLES_ALLOCATOR
    if (bytes > maximum_chunk_size()) {
#endif
        native_dealloc(block, bytes); // Revert to the C standard deallocator
        return;
#ifdef OODLES_ALLOCATOR
    }
#endif

    /*
     * n byte-sized objects of this word alignment will
     * fit into the same fixed-sized allocator
     */
    const size_t index = ((bytes + (alignment() - 1)) / alignment()) - 1;
    deallocator = &allocators[index];

    assert(deallocator->size() == bytes);

#ifdef DEBUG_ALLOCATOR
    std::cerr << "DEBUG[allocator]: ChunkManager "
              << deallocator << '/' << index
              << " services deallocations for "
              << bytes << "b objects.\n";
#endif

    deallocator->deallocate(block);
}

ObjectAllocator&
ObjectAllocator::instance()
{
    static ObjectAllocator x;
    return x;
}

} // allocator
} // oodles
