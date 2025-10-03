/**
 * @file StringConversion.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for converting numbers/floats to Strings and Strings to 
 *    numbers/floats.
 * @version 0.1
 * @date 09-29-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/TypeCheck.hpp"
#include "tundra/utils/containers/String.hpp"


namespace Tundra
{

namespace Internal
{

Tundra::Str::String int8_to_string(Tundra::int8 num);
Tundra::Str::String uint8_to_string(Tundra::uint8 num);
Tundra::Str::String int16_to_string(Tundra::int16 num);
Tundra::Str::String uint16_to_string(Tundra::uint16 num);
Tundra::Str::String int32_to_string(Tundra::int32 num);
Tundra::Str::String uint32_to_string(Tundra::uint32 num);
Tundra::Str::String int64_to_string(Tundra::int64 num);
Tundra::Str::String uin64_to_string(Tundra::uint64 num);
Tundra::Str::String float_to_string(float num);
Tundra::Str::String double_to_string(double num);

Tundra::int8 string_to_int8(const Tundra::Str::String *str);
Tundra::uint8 string_to_uint8(const Tundra::Str::String *str);
Tundra::int16 string_to_int16(const Tundra::Str::String *str);
Tundra::uint16 string_to_uint16(const Tundra::Str::String *str);
Tundra::int32 string_to_int32(const Tundra::Str::String *str);
Tundra::uint32 string_to_uint32(const Tundra::Str::String *str);
Tundra::int64 string_to_int64(const Tundra::Str::String *str);
Tundra::uint64 string_to_uint64(const Tundra::Str::String *str);
float string_to_float(const Tundra::Str::String *str);
double string_to_double(const Tundra::Str::String *str);


} // namespace Internal


/**
 * @brief Converts a String to a number of the specified type.
 *
 * Supports all arithmetic types, including signed and unsigned integers, 
 * as well as floating-point types.
 *
 * @tparam T Target numeric type.
 * @param str Read-only pointer to the String.
 *
 * @return T The converted numeric value.
 */
template<typename T>
T string_to_num(const Tundra::Str::String *str)
{
    if constexpr (Tundra::is_matching_type<T, Tundra::int8>::value)
    {
        return Tundra::Internal::string_to_int8(str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::uint8>::value)
    {
        return Tundra::Internal::string_to_uint8(str);
    }

    // Handle other types....

    else
    {
        static_assert(Tundra::always_false<T>::value, "Only arithmetic types\
            are supported for conversion.");
    }
}

/**
 * @brief Converts a number into its readable form. 
 * 
 * Supports all arithmetic types, including signed and unsigned integers, 
 * as well as floating-point types.
 *
 * @tparam T Target numeric type.
 * @param num Num to convert.
 *
 * @return Tundra::Str::String Constructed String representing the number. 
 */
template<typename T>
Tundra::Str::String num_to_string(T num) 
{

    if constexpr(Tundra::is_matching_type<T, Tundra::int8>::value)
    {
        return Tundra::Internal::int8_to_string(num);
    }

    // Handle other types....

    else
    {
        static_assert(Tundra::always_false<T>::value, "Only arithmetic types\
            are supported for conversion.");
    }
}

} // namespace Tundra
