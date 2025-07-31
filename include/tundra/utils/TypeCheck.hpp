/**
 * @file TypeCheck.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Compile time type checking through templated structs.
 * @version 0.1
 * @date 07-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"

namespace Tundra
{

namespace Str
{
// Forward Declaration
template<uint8 alignment>
struct String;

} // namespace Str


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

// Primary: not a String
template<typename>
struct is_string 
{
    static constexpr bool value = false;
};

// Specialization: it is a String<alignment>
template<uint8 alignment>
struct is_string<Tundra::Str::String<alignment>> 
{
    static constexpr bool value = true;
};

} // namespace Tundra

