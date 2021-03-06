
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// A general purpose hash function for non-zero floating point values.

#if !defined(CPPCMS_BOOST_FUNCTIONAL_HASH_DETAIL_HASH_FLOAT_GENERIC_HEADER)
#define CPPCMS_BOOST_FUNCTIONAL_HASH_DETAIL_HASH_FLOAT_GENERIC_HEADER

#include <cppcms_boost/functional/hash/detail/float_functions.hpp>
#include <cppcms_boost/integer/static_log2.hpp>
#include <cppcms_boost/functional/hash/detail/limits.hpp>

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#if defined(CPPCMS_BOOST_MSVC)
#pragma warning(push)
#if CPPCMS_BOOST_MSVC >= 1400
#pragma warning(disable:6294) // Ill-defined for-loop: initial condition does
                              // not satisfy test. Loop body not executed 
#endif
#endif

namespace cppcms_boost
{
    namespace hash_detail
    {
        inline void hash_float_combine(std::size_t& seed, std::size_t value)
        {
            seed ^= value + (seed<<6) + (seed>>2);
        }

        template <class T>
        inline std::size_t float_hash_impl2(T v)
        {
            cppcms_boost::hash_detail::call_frexp<T> frexp;
            cppcms_boost::hash_detail::call_ldexp<T> ldexp;
        
            int exp = 0;

            v = frexp(v, &exp);

            // A postive value is easier to hash, so combine the
            // sign with the exponent and use the absolute value.
            if(v < 0) {
                v = -v;
                exp += limits<T>::max_exponent -
                    limits<T>::min_exponent;
            }

            // The result of frexp is always between 0.5 and 1, so its
            // top bit will always be 1. Subtract by 0.5 to remove that.
            v -= T(0.5);
            v = ldexp(v, limits<std::size_t>::digits + 1);
            std::size_t seed = static_cast<std::size_t>(v);
            v -= seed;

            // ceiling(digits(T) * log2(radix(T))/ digits(size_t)) - 1;
            std::size_t const length
                = (limits<T>::digits *
                        cppcms_boost::static_log2<limits<T>::radix>::value - 1)
                / limits<std::size_t>::digits;

            for(std::size_t i = 0; i != length; ++i)
            {
                v = ldexp(v, limits<std::size_t>::digits);
                std::size_t part = static_cast<std::size_t>(v);
                v -= part;
                hash_float_combine(seed, part);
            }

            hash_float_combine(seed, exp);

            return seed;
        }

        template <class T>
        inline std::size_t float_hash_impl(T v)
        {
            typedef CPPCMS_BOOST_DEDUCED_TYPENAME select_hash_type<T>::type type;
            return float_hash_impl2(static_cast<type>(v));
        }
    }
}

#if defined(CPPCMS_BOOST_MSVC)
#pragma warning(pop)
#endif

#endif
