#ifndef OODLES_NET_OOP_BOOST_HPP
#define OODLES_NET_OOP_BOOST_HPP

// oodles
#include "Accessor.ipp"

// Boost.serialization
#include <boost/serialization/split_free.hpp>

/*
 * This file holds the non-intrusive serialize free-functions
 * for oodles types in the boost::serialization namespace. To
 * permit the boost::serialization library access to private
 * class members in oodles types each serializable class
 * definition should include "Accessor.hpp" and use the macro
 * PERMIT_SERIALISER_ACCESS in the private visibility section.
 */
namespace boost {
namespace serialization {

// URL
template<class Archive>
void
load(Archive &archive, oodles::url::URL &url, unsigned int version)
{
    oodles::net::oop::Accessor::load<Archive>(archive, url, version);
}

template<class Archive>
void
save(Archive &archive, const oodles::url::URL &url, unsigned int version)
{
    oodles::net::oop::Accessor::save<Archive>(archive, url, version);
}

} // serialization
} // boost

BOOST_SERIALIZATION_SPLIT_FREE(oodles::url::URL)

#endif

