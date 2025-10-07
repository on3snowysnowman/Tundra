/**
 * @file StringConversion.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for converting numbers/floats to Strings and Strings to 
 *    numbers/floats.
 * @version 0.1
 * @date 09-29-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/TypeCheck.hpp"
#include "tundra/utils/NumericLimits.hpp"
#include "tundra/utils/containers/String.hpp"

namespace Tundra
{

namespace Internal
{
void int8_to_string(Tundra::int8 num, Tundra::Str::String *str);
void uint8_to_string(Tundra::uint8 num, Tundra::Str::String *str);
void int16_to_string(Tundra::int16 num, Tundra::Str::String *str);
void uint16_to_string(Tundra::uint16 num, Tundra::Str::String *str);
void int32_to_string(Tundra::int32 num, Tundra::Str::String *str);
void uint32_to_string(Tundra::uint32 num, Tundra::Str::String *str);
void int64_to_string(Tundra::int64 num, Tundra::Str::String *str);
void uint64_to_string(Tundra::uint64 num, Tundra::Str::String *str);
void float_to_string(float num, Tundra::Str::String *str);
void double_to_string(double num, Tundra::Str::String *str);


/**
 * @brief Converts a String to a specified integral type. 
 *
 * If the specified type would overflow from the digits in the String, the 
 * maximum (or minimum, if negative) limit is returned of the type.
 * 
 * @tparam T Integer type to convert to.
 *
 * @param str Read-only pointer to the String.
 * 
 * @return T Converted number.
 */
template<typename T> 
T convert_str_to_int_type(const Tundra::Str::String *str)
{
    static constexpr int BASE_TEN = 10;
    static constexpr int MAX_DGT_CNT_UINT64 = 20;

    Tundra::uint64 STR_SIZE = Tundra::Str::size(str);

    if(STR_SIZE == 0) 
        { return 0; }

    Tundra::uint64 accumulator = 0;

    bool is_negative = false;

    Tundra::uint64 it = 0;

    if(*Tundra::Str::peek_unchecked(str, 0) == '-')
    {
        is_negative = true;
        ++it;
    }

    Tundra::uint64 last_accumulator = 0;

    while(it < STR_SIZE || it > MAX_DGT_CNT_UINT64)
    {
        const char *parsed_char = Tundra::Str::peek_unchecked(str, it);

        if(*parsed_char < '0' || *parsed_char > '9')
        {
            return 0;
        }

        accumulator *= BASE_TEN;
        accumulator += (*parsed_char - '0');

        // If we've reduced our accumulator, we've overflowed.
        if(accumulator < last_accumulator)
        {
            if(is_negative) { return Tundra::NumericLimits<T>::min; }
            return Tundra::NumericLimits<T>::max;
        }

        last_accumulator = accumulator;

        ++it;
    }

    if(is_negative)
    {
        Tundra::int64 neg_accumulator = (accumulator > 
            (Tundra::uint64)Tundra::NumericLimits<Tundra::int64>::max + 1) ?
            Tundra::NumericLimits<Tundra::int64>::min : 
            (Tundra::int64)-accumulator;

        T final_num = (neg_accumulator < Tundra::NumericLimits<T>::min) ? 
            Tundra::NumericLimits<T>::min : neg_accumulator;
        
        return final_num;
    }

    return (accumulator > Tundra::NumericLimits<T>::max) ? 
        Tundra::NumericLimits<T>::max : (T)accumulator;
}

// float string_to_float(const Tundra::Str::String *str);
// double string_to_double(const Tundra::Str::String *str);


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
    static_assert(Tundra::is_integral_type<T>::value, "Only arithmetic types\
         are supported for conversion.\n");

    return Tundra::Internal::convert_str_to_int_type<T>(str);
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
    Tundra::Str::String str;
    Tundra::Str::init(&str);

    if constexpr(Tundra::is_matching_type<T, Tundra::int8>::value)
    {
        Tundra::Internal::int8_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::uint8>::value)
    {
        Tundra::Internal::uint8_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::int16>::value)
    {
        Tundra::Internal::int16_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::uint16>::value)
    {
        Tundra::Internal::uint16_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::int32>::value)
    {
        Tundra::Internal::int32_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::uint32>::value)
    {
        Tundra::Internal::uint32_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::int64>::value)
    {
        Tundra::Internal::int64_to_string(num, &str);
    }

    else if constexpr (Tundra::is_matching_type<T, Tundra::uint64>::value)
    {
        Tundra::Internal::uint64_to_string(num, &str);
    }

    // Type not supported.
    else
    {
        static_assert(Tundra::always_false<T>::value, "Only arithmetic types\
            are supported for conversion.");
    }

    return str;
}

} // namespace Tundra
