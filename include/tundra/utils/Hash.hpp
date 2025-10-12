/**
 * @file Hash.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Hash functions for general types and String.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/TypeCheck.hpp"
#include "tundra/utils/containers/String.hpp"


namespace Tundra
{

namespace Internal
{

inline uint64 hash_uint64(uint64 i)
{
    i ^= i >> 33;
    i *= 0xff51afd7ed558ccdULL;
    i ^= i >> 33;
    i *= 0xc4ceb9fe1a85ec53ULL;
    i ^= i >> 33;
    return i;
}

} // namespace Internal

/**
 * @brief Computes a 64-bit hash for the given value.
 *
 * Supports hashing of basic integral types (e.g., uint8, int32, uint64),
 * as well as Strings.
 *
 * @tparam T Type of the value to hash. Must be a supported type.
 * @param value Value to hash.
 * @return uint64 Hashed 64-bit value.
 *
 * @note This function requires that a valid overload or specialization exists
 * for the provided type. If unsupported, a static assertion will be triggered.
 */
template<typename T>
inline uint64 hash(const T& value)
{
    if constexpr (is_matching_type<T, uint8>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(value));
    }

    else if constexpr (is_matching_type<T, int8>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(uint8)(value));
    }

    else if constexpr (is_matching_type<T, uint16>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(value));
    }

    else if constexpr (is_matching_type<T, int16>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(uint16)(value));
    }

    else if constexpr (is_matching_type<T, uint32>::value)
    {
        return Tundra::Internal::hash_uint64((uint32)(value));
    }

    else if constexpr (is_matching_type<T, int32>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(uint32)(value));
    }

    else if constexpr (is_matching_type<T, uint64>::value)
    {
        return Tundra::Internal::hash_uint64(value);
    }

    else if constexpr (is_matching_type<T, int64>::value)
    {
        return Tundra::Internal::hash_uint64((uint64)(value));
    }

    else if constexpr (is_string<T>::value)
    {
        return Tundra::Str::hash(value);
    }

    else 
    {
        static_assert(Tundra::always_false<int>::value, "No standard hash\
            function for the provided type.");
    }
}

} // namespace Tundra
