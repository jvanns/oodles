// oodles
#include "BitSet.hpp"

// libc
#include <stdlib.h> // For calloc()
#include <limits.h> // For UINT32_MAX

#ifndef UINT32_MAX
#define UINT32_MAX ((uint32_t)(-1))
#endif

namespace {

// These patterns form the magic of our population count (bit count)
const uint32_t MASK_01010101 = UINT32_MAX / 3; // 0x55555555
const uint32_t MASK_00110011 = UINT32_MAX / 5; // 0x33333333
const uint32_t MASK_00001111 = UINT32_MAX / 17; // 0xF0F0F0F0

const uint8_t BYTE_SIZE = 8;
const uint8_t VALUE_SIZE = sizeof(uint32_t);
const uint16_t BITS_PER_VALUE = VALUE_SIZE * BYTE_SIZE;
const uint16_t BITWISE_MODULO = BITS_PER_VALUE - 1; // A Mersenne prime

// Population count counts non-zero bits in a value
inline
uint32_t population_count(uint32_t value)
{
    /* The following 3 lines should evaluate to 5 instructions each */
    value = (value & MASK_01010101) + ((value >> 1) & MASK_01010101);
    value = (value & MASK_00110011) + ((value >> 2) & MASK_00110011);
    value = (value & MASK_00001111) + ((value >> 4) & MASK_00001111);

    return value % 255;
}

// Population count counts non-zero bits in a value up to the nth bit
inline
uint32_t population_count(uint32_t value, uint32_t limit)
{
    if (limit == 0)
        return 0;

    value <<= (BITS_PER_VALUE - limit);

    return population_count(value);
}

inline
void
set_bits(uint32_t *set, uint32_t value)
{
    set[value / BITS_PER_VALUE] |= (1 << (value & BITWISE_MODULO));
}

inline
void
clear_bits(uint32_t *set, uint32_t value)
{
    set[value / BITS_PER_VALUE] &= ~(1 << (value & BITWISE_MODULO));
}

inline
bool
has_value(uint32_t *set, uint32_t value)
{
    return set[value / BITS_PER_VALUE] & (1 << (value & BITWISE_MODULO));
}

} // anonymous

namespace oodles {

BitSet::BitSet(uint32_t max) :
    max(max),
    set((uint32_t*)calloc((max / BITS_PER_VALUE) + 1, BYTE_SIZE))
{
}

BitSet::~BitSet()
{
    free(set);
}

bool
BitSet::has_value(uint32_t value) const
{
    if (value > max)
        return false;

    return ::has_value(set, value);
}

bool
BitSet::set_bits(uint32_t value)
{
    if (value > max || has_value(value))
        return false;

    ::set_bits(set, value);

    return true;
}

bool
BitSet::clear_bits(uint32_t value)
{
    if (!has_value(value))
        return false;

    ::clear_bits(set, value);

    return true;
}

int32_t
BitSet::index_of(uint32_t value) const
{
    if (!has_value(value))
        return -1;

    uint32_t i = 0, j = value / BITS_PER_VALUE, k = 0;

    while (i < j)
        k += population_count(set[i++]);

    i = value & BITWISE_MODULO;
    k += population_count(set[j], i);

    return k;
}

} // oodles
