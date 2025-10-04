/**
 * @file StringConversion.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 09-29-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/utils/StringConversion.hpp"
#include "tundra/utils/containers/String.hpp"
#include "tundra/utils/CoreTypes.hpp"

constexpr char DIGITS_LUT[] =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";

constexpr Tundra::uint8 MAX_DIGITS_TO_REPRESENT_INT8 = 3;
constexpr Tundra::uint8 MAX_DIGITS_TO_REPRESENT_INT16 = 5;
constexpr Tundra::uint8 MAX_DIGITS_TO_REPRESENT_INT32 = 10;
constexpr Tundra::uint8 MAX_DIGITS_TO_REPRESENT_INT64 = 20;

template<typename T, Tundra::uint8 max_buff_size>
void underlying_convert_num_to_string(T num, Tundra::Str::String *str)
{
    char buffer[max_buff_size];
    char *it = buffer + max_buff_size;
    char *end = it;
    
    while(num >= 100)
    {
        T quotient = (T)(num / 100);
        T remainder = (T)(num - (quotient * 100));
        
        const char *DIGITS = &DIGITS_LUT[(remainder << 1)];

        *--it = DIGITS[1];
        *--it = DIGITS[0];
        num = quotient;
    }

    // Final 1-2 digits.
    if(num < 10)
    {
        *--it = (char)('0' + num);
    }

    else
    {
        const char *DIGITS = &DIGITS_LUT[(num << 1)];
        *--it = DIGITS[1];
        *--it = DIGITS[0];
    }

    Tundra::Str::add_chars(str, it, end - it);
}

void Tundra::Internal::int8_to_string(Tundra::int8 num,
    Tundra::Str::String *str)
{
    Tundra::uint8 v = (Tundra::uint8)num;

    if(num >= 0) 
    { 
        underlying_convert_num_to_string<Tundra::uint8, 
            MAX_DIGITS_TO_REPRESENT_INT8>(v, str);
        return;
    }
    
    Tundra::Str::add_char(str, '-');

    // Negate bits and add 1 to convert to a positive number.
    // v = ~v + 1U;
    v = -v;

    // Tundra::Internal::uint8_to_string(v, str);
    underlying_convert_num_to_string<Tundra::uint8, 
        MAX_DIGITS_TO_REPRESENT_INT8>(v, str);
}

void Tundra::Internal::uint8_to_string(Tundra::uint8 num,
    Tundra::Str::String *str)
{
    underlying_convert_num_to_string<Tundra::uint8, 
        MAX_DIGITS_TO_REPRESENT_INT8>(num, str);
}

void Tundra::Internal::int16_to_string(Tundra::int16 num,
    Tundra::Str::String *str)
{    
    Tundra::uint16 v = (Tundra::uint16)num;

    if(num >= 0)
    {
        // Tundra::Internal::uint16_to_string(num, str);
        underlying_convert_num_to_string<Tundra::uint16, 
            MAX_DIGITS_TO_REPRESENT_INT16>(v, str);
        return;
    }

    Tundra::Str::add_char(str, '-');

    // Negate bits and add 1 to convert to a positive number.
    // v = ~v + 1U;
    v = -v;

    underlying_convert_num_to_string<Tundra::uint16, 
        MAX_DIGITS_TO_REPRESENT_INT16>(v, str);
}

void Tundra::Internal::uint16_to_string(Tundra::uint16 num,
    Tundra::Str::String *str)
{
    underlying_convert_num_to_string<Tundra::uint16, 
        MAX_DIGITS_TO_REPRESENT_INT16>(num, str);
}

void Tundra::Internal::int32_to_string(Tundra::int32 num,
    Tundra::Str::String *str)
{
    Tundra::uint32 v = (Tundra::uint32)num;

    if(num >= 0)
    {
        underlying_convert_num_to_string<Tundra::uint32, 
            MAX_DIGITS_TO_REPRESENT_INT32>(v, str);
        return;
    }

    Tundra::Str::add_char(str, '-');
    
    // Negate bits and add 1 to convert to a positive number.
    // v = ~v + 1U;
    v = -v;

    underlying_convert_num_to_string<Tundra::uint32, 
        MAX_DIGITS_TO_REPRESENT_INT32>(v, str);
}

void Tundra::Internal::uint32_to_string(Tundra::uint32 num,
    Tundra::Str::String *str)
{
    underlying_convert_num_to_string<Tundra::uint32, 
        MAX_DIGITS_TO_REPRESENT_INT32>(num, str);
}

void Tundra::Internal::int64_to_string(Tundra::int64 num,
    Tundra::Str::String *str)
{
    Tundra::uint64 v = (Tundra::uint64)num;

    if(num >= 0)
    {
        underlying_convert_num_to_string<Tundra::uint64, 
            MAX_DIGITS_TO_REPRESENT_INT64>(v, str);
        return;
    }

    Tundra::Str::add_char(str, '-');

    // Negate bits and add 1 to convert to a positive number.
    // v = ~v + 1U;
    v = -v;

    underlying_convert_num_to_string<Tundra::uint64, 
        MAX_DIGITS_TO_REPRESENT_INT64>(v, str);
}

void Tundra::Internal::uint64_to_string(Tundra::uint64 num,
    Tundra::Str::String *str)
{
    underlying_convert_num_to_string<Tundra::uint64, 
        MAX_DIGITS_TO_REPRESENT_INT64>(num, str);
}

// #include <iostream>
// Tundra::int8 Tundra::Internal::string_to_int8(const Tundra::Str::String *str)
// {
//     const Tundra::uint64 STR_SIZE = Tundra::Str::size(str);

//     if(STR_SIZE == 0) { return 0; }

//     int accumulator = 0;

//     // Start at the char to the far right.
//     Tundra::uint64 it = STR_SIZE - 1;

//     const char *parsed_char = 0;

//     while(it > 0)
//     {
//         parsed_char = Tundra::Str::peek_unchecked(str, it);

//         // If this digit is not a number.
//         if(*parsed_char < '0' || *parsed_char > '9')
//         {
//             ;
//         }

//         std::cout << *parsed_char << '\n';
//         --it;
//     }

//     parsed_char = Tundra::Str::peek_unchecked(str, it);

//     if(*parsed_char == '-')
//     {
//         return (Tundra::int8)-num;
//     }

//     return num;
// }

