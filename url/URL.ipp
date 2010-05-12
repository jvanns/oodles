#ifndef OODLES_URL_URL_IPP // Implementation
#define OODLES_URL_URL_IPP

#include "utility/hash.hpp"

namespace oodles {
namespace url {

// Define a local-only template function, hasher
template<class Container>
URL::hash_t
hasher(const Container &input, URL::hash_t seed);

// Provide the two necessary specialisations of hasher:
template<>
inline
URL::hash_t
hasher<value_type>(const value_type &input, URL::hash_t seed)
{
    return fnv64(input.data(), input.size(), seed);
}

template<>
inline
URL::hash_t
hasher<std::vector<value_type> >(const std::vector<value_type> &input,
                                                     URL::hash_t seed)
{
    std::vector<value_type>::const_iterator i = input.begin();

    for ( ; i != input.end() ; ++i)
        seed = hasher(*i, seed);

    return seed;
}

template<class Type>
URL::IDGenerator<Type>::IDGenerator(const Type &v) : hash(0), content(v)
{
}

template<class Type>
URL::IDGenerator<Type>::~IDGenerator()
{
}

template<class Type>
URL::hash_t
URL::IDGenerator<Type>::compute_hash(hash_t seed)
{
    if (!hash)
        hash = hasher(content, seed);

    return hash;
}

} // url
} // oodles

#endif

