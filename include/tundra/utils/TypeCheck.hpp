/**
 * @file TypeCheck.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Type and value checking through templated structs and functions.
 * @version 0.1
 * @date 07-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/containers/String.hpp"

namespace Tundra
{

template<typename T>
struct is_integral_type
{
    static constexpr bool value = false;
};

template<>
struct is_integral_type<Tundra::int8>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::uint8>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::int16>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::uint16>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::int32>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::uint32>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::int64>
{
    static constexpr bool value = true;
};

template<>
struct is_integral_type<Tundra::uint64>
{
    static constexpr bool value = true;
};


/**
 * @brief Type trait to check if two types are the same.
 * 
 * This is the primary template, which defaults to `false`.
 * Specialization is provided for when both types are the same.
 * 
 * @tparam T First type
 * @tparam U Second type
 */
template<typename T, typename U>
struct is_matching_type
{
    static constexpr bool value = false;
};

/**
 * @brief Specialization for when both types are the same.
 * 
 * @tparam T The type being compared to itself.
 */
template<typename T>
struct is_matching_type<T, T>
{
    static constexpr bool value = true;
};

/**
 * @brief Type trait to check if a type is a String.
 *
 * This is the primary template, which defaults to not a String. Specialization
 * is provided for when `T` is a String.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_string 
{
    static constexpr bool value = false;
};

/**
 * @brief Specialization for when the passed type `T` is a String.
 * 
 * @tparam Type that is a String.
 */
template<>
struct is_string<Tundra::Str::String> 
{
    static constexpr bool value = true;
};

/**
 * @brief Checks if the pointed to value `first` is equal to the pointed to
 * `second`, returning true if they are equal.
 *
 * Supports checking of basic integral types (e.g., uint8, int32, uint64),
 * as well as Strings.
 * 
 * @tparam T Type of the pointed to objects to compare. Must be supported type.
 * @param first Pointer to the first object.
 * @param second Pointer to the second object.
 * @return bool True if the values are equal. 
 */
template<typename T>
bool is_matching_value(const T &first, const T &second)
{
    if constexpr(is_integral_type<T>::value)
    {
        return first == second;
    }

    else if constexpr(is_string<T>::value)
    {
        return Tundra::Str::compare(first, second);
    }

    else
    {
        static_assert(Tundra::always_false<T>::value, "No standard compare\
            function for the provided type.");
    }
}


} // namespace Tundra

