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


template<typename T>
inline T convert_str_to_int_type_neg(const Tundra::Str::String *str, 
    const Tundra::uint64 STR_SIZE)
{
    if(STR_SIZE == 1) { return Tundra::NumericLimits<T>::min; }

    // -- The first character of the String has already been parsed as '-', so 
    // we start at index 1 in the String.
    
    static constexpr T BASE_TEN = 10;
    
    T converted_num = 0;

    for(Tundra::uint64 i = 1; i < STR_SIZE; ++i)
    {
        const char parsed_char = Tundra::Str::at(*str, i);

        if(parsed_char < '0' || parsed_char > '9')
        {
            return Tundra::NumericLimits<T>::min;
        }

        // Multiply the currently converted number by 10 
        // since we're parsing a new digit in the next spot to the right, and 
        // want to shift all the digits left one. Then, subtract the parsed 
        // digit (since we're negative). We check if either of these operations 
        // overflow.
        if(Tundra::mul_check_overflow(converted_num, BASE_TEN) || 
            Tundra::sub_check_overflow(converted_num, 
                static_cast<T>(parsed_char - '0')))
        {
            return Tundra::NumericLimits<T>::min; 
        }
    }

    return converted_num;
}

template<typename T>
inline T convert_str_to_int_type(const Tundra::Str::String *str)
{
    static constexpr T BASE_TEN = 10;
    
    Tundra::uint64 STR_SIZE = Tundra::Str::size(*str);

    if(STR_SIZE == 0) { return Tundra::NumericLimits<T>::max; }

if constexpr(Tundra::is_signed_integer<T>::value)
{
    if(Tundra::Str::at(*str, 0) == '-')
    { 
        return Tundra::Internal::convert_str_to_int_type_neg<T>(str, STR_SIZE);
    }
}

    // -- Past this point, we know the number in the String is positive. -- 

    T converted_num = 0;

    for(Tundra::uint64 i = 0; i < STR_SIZE; ++i)
    {
        const char parsed_char = Tundra::Str::at(*str, i);

        if(parsed_char < '0' || parsed_char > '9')
        {
            return Tundra::NumericLimits<T>::max;
        }

        // Multiply the currently converted number by 10 
        // since we're parsing a new digit in the next spot to the right, and 
        // want to shift all the digits left one. Then, add the parsed digit. We 
        // check if either of these operations overflow.
        if(Tundra::mul_check_overflow(converted_num, BASE_TEN) || 
            Tundra::add_check_overflow(converted_num, 
                static_cast<T>(parsed_char - '0')))
        {
            return Tundra::NumericLimits<T>::max; 
        }
    }

    return converted_num;
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
inline T str_to_num(const Tundra::Str::String *str)
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
inline Tundra::Str::String num_to_str(T num) 
{
    Tundra::Str::String str;
    Tundra::Str::init(str);

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
