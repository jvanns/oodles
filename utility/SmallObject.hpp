#ifndef OODLES_ALLOCATOR_SMALLOBJECT_HPP
#define OODLES_ALLOCATOR_SMALLOBJECT_HPP

// oodles
#include "ObjectAllocator.hpp"

namespace oodles {
namespace allocator {

class SmallObject
{
    public:
        /* Member functions/methods */
        virtual ~SmallObject() = 0;

        static inline void* operator new (size_t bytes)
        {
            return ObjectAllocator::instance().allocate(bytes);
        }

        static inline void operator delete (void *block, size_t bytes)
        {
            ObjectAllocator::instance().deallocate(block, bytes);
        }
    protected:
        /* Member functions/methods */
        SmallObject() {}
    private:
        /* Member functions/methods */
        SmallObject(const SmallObject &o);
        SmallObject& operator= (const SmallObject &o);
};

inline
SmallObject::~SmallObject()
{
}

} // allocator
} // oodles

#endif

