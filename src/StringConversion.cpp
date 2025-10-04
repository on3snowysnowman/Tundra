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

static const char* DIGITS_LUT =
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


void Tundra::Internal::int8_to_string(Tundra::int8 num,
    Tundra::Str::String *str)
{
    Tundra::uint8 v = (Tundra::uint8)num;

    if(num >= 0) 
    { 
        Tundra::Internal::uint8_to_string(v, str); 
        return;
    }
    
    Tundra::Str::add_char(str, '-');
    
    // Negate bits and add 1 to convert to a positive number.
    v = (Tundra::uint8)(~v) + 1U;
    Tundra::Internal::uint8_to_string(v, str);
}

void Tundra::Internal::uint8_to_string(Tundra::uint8 num,
    Tundra::Str::String *str)
{
    // char buffer[3];

    // // Start placing numbers at the visual beginning of the buffer.
    // char *it = buffer + 3;
    // char *end_buff = it;

    if(num >= 100)
    {
        // Get hundredths place.
        Tundra::uint8 tenth_digit = num / 100;
        Tundra::uint8 remainder = (Tundra::uint8)(num - (tenth_digit * 100));
        
        // const char *const DIGITS = &DIGITS_LUT[(remainder << 1)];

        Tundra::Str::add_char(str, (char)('0' + tenth_digit));
        Tundra::Str::add_chars(str, &DIGITS_LUT[(remainder << 1)], 2);
        return;
        // *--it = DIGITS[1];
        // *--it = DIGITS[0];
        // *--it = (char)('0' + tenth_digit);
    }

    else if(num >= 10)
    {
        Tundra::Str::add_chars(str, &DIGITS_LUT[(num << 1)], 2);
        return;

        // const char *const DIGITS = &DIGITS_LUT[(num << 1)];
        // *--it = DIGITS[1];
        // *--it 
    }

    // -- num < 10 --
    Tundra::Str::add_char(str, (char)('0' + num));
}

// void Tundra::Internal::int16_to_string(Tundra::int16 num,
//     Tundra::Str::String *str)
// {
//     Tundra::uint16 v = (Tundra::uint16)num;
    
//     if(num >= 0)
//     {
//         Tundra::Internal::uint16_to_string(num, str);
//         return;
//     }

//     Tundra::Str::add_char(str, '-');

//     // Negate bits and add 1 to convert to a positive number.
//     v = (Tundra::uint16)(~v) + 1U;
//     Tundra::Internal::uint16_to_string(num, str);
// }

