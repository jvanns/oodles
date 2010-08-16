#ifndef OODLES_BITSET_HPP
#define OODLES_BITSET_HPP

// libc
#include <stdint.h> // For uint32_t

namespace oodles {

class BitSet
{
    public:
        /* Member functions/methods */
        BitSet(uint32_t max);
        ~BitSet();

        bool set_bits(uint32_t value);
        bool clear_bits(uint32_t value);
        bool has_value(uint32_t value) const;
        int32_t index_of(uint32_t value) const;
    private:
        /* Member variables/attributes */
        uint32_t max;
        uint32_t *set;
};

} // oodles

#endif

