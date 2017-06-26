// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
#ifndef HEPMC_COMMON_H
#define HEPMC_COMMON_H

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>

namespace HepMC {


    // Use unqualified namespace std within the HepMC namespace
    using namespace std;


    /// Handy number squaring function
    template <typename NUM>
    inline NUM sqr(NUM x) { return x*x; }


    /** @brief List of methods of searching starting from a particle or vertex
        @todo Add 'family'='parents'+'children' and 'relatives'='ancestors'+'descendants'(+'siblings')?
    */
    enum Relationship {
      ANCESTORS = 0, FIND_ANCESTORS = 0, FIND_ALL_ANCESTORS = 0, ancestors = 0,
      DESCENDANTS = 1, FIND_DESCENDANTS = 1, FIND_ALL_DESCENDANTS = 1, descendants = 1,
      PARENTS = 2, FIND_PARENTS = 2, FIND_MOTHERS = 2, parents = 2,
      CHILDREN = 3, FIND_CHILDREN = 3, FIND_DAUGHTERS = 3, children = 3,
      PRODUCTION_SIBLINGS = 4, FIND_PRODUCTION_SIBLINGS = 4
    };

    /// Compatibility name
    using FilterParticle = Relationship;
    #ifndef HEPMC_NO_DEPRECATED
    /// Compatibility name
    using IteratorRange = Relationship;
    #endif

}


/// Neater/extensible C++11 availability test
#if __cplusplus >= 201103L
#define HEPMC_HAS_CXX11
#endif
#if !defined(HEPMC_HAS_CXX11) && (__GNUC__) && (__cplusplus) && (__GXX_EXPERIMENTAL_CXX0X__)
#define HEPMC_HAS_CXX0X_GCC_ONLY
#endif


/// Define a FOREACH directive
#ifdef  HEPMC_HAS_CXX11
#define FOREACH( iterator, container ) for ( iterator: container )
#else
#if !defined(__CINT__)
/* This code was adapted from
 * https://github.com/assimp/assimp/blob/master/code/BoostWorkaround/boost/foreach.hpp
 * to rid of boost dependency.
 */
///////////////////////////////////////////////////////////////////////////////
// A stripped down version of FOREACH for
// illustration purposes. NOT FOR GENERAL USE.
// For a complete implementation, see BOOST_FOREACH at
// http://boost-sandbox.sourceforge.net/vault/index.php?directory=eric_niebler
//
// Copyright 2004 Eric Niebler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Adapted to Assimp November 29th, 2008 (Alexander Gessler).
// Added code to handle both const and non-const iterators, simplified some
// parts.
//////////////////////////////////////////////////////////////////////////////
namespace hepmc_embedded_boost {
namespace foreach_detail {

///////////////////////////////////////////////////////////////////////////////
// auto_any

struct auto_any_base
{
    operator bool() const { return false; }
};

template<typename T>
struct auto_any : auto_any_base
{
    auto_any(T const& t) : item(t) {}
    mutable T item;
};

template<typename T>
T& auto_any_cast(auto_any_base const& any)
{
    return static_cast<auto_any<T> const&>(any).item;
}

///////////////////////////////////////////////////////////////////////////////
// FOREACH helper function

template<typename T>
auto_any<typename T::const_iterator> begin(T const& t)
{
    return t.begin();
}

template<typename T>
auto_any<typename T::const_iterator> end(T const& t)
{
    return t.end();
}

// iterator
template<typename T>
bool done(auto_any_base const& cur, auto_any_base const& end, T&)
{
    typedef typename T::iterator iter_type;
    return auto_any_cast<iter_type>(cur) == auto_any_cast<iter_type>(end);
}

template<typename T>
void next(auto_any_base const& cur, T&)
{
    ++auto_any_cast<typename T::iterator>(cur);
}

template<typename T>
typename T::reference deref(auto_any_base const& cur, T&)
{
    return *auto_any_cast<typename T::iterator>(cur);
}

} // end foreach_detail

///////////////////////////////////////////////////////////////////////////////
// FOREACH

#define FOREACH(item, container)                      \
	if(hepmc_embedded_boost::foreach_detail::auto_any_base const& b = hepmc_embedded_boost::foreach_detail::begin(container)) {} else       \
    if(hepmc_embedded_boost::foreach_detail::auto_any_base const& e = hepmc_embedded_boost::foreach_detail::end(container))   {} else       \
    for(;!hepmc_embedded_boost::foreach_detail::done(b,e,container);  hepmc_embedded_boost::foreach_detail::next(b,container))   \
        if (bool ugly_and_unique_break = false) {} else							\
        for(item = hepmc_embedded_boost::foreach_detail::deref(b,container); !ugly_and_unique_break; ugly_and_unique_break = true)

} // end hepmc_embedded_boost
#endif
#endif


/// Deprecation macro
#ifndef HEPMC_DEPRECATED
#define HEPMC_DEPRECATED(x)
#endif
/// @todo Activate in version 3.1.0
// #ifndef HEPMC_DEPRECATED
// #if __GNUC__ && __cplusplus && HEPMC_NO_DEPRECATION_WARNINGS == 0
// #define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
// #if GCC_VERSION >= 40500
//   #define HEPMC_DEPRECATED(x) __attribute__((deprecated(x)))
// #else
//   #define HEPMC_DEPRECATED(x) __attribute__((deprecated))
// #endif
// #else
//   #define HEPMC_DEPRECATED(x)
// #endif
// #endif


#endif
