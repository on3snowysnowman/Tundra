/**
 * @file ToString.c
 * @author your name (you@domain.com)
 * @brief Methods for converting numbers to c strings.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/ToString.h"

Tundra_ToStrResult Tundra_u64_to_str(u64 num)
{
    Tundra_ToStrResult result;

    if (num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_DIGITS_FOR_U64 + 1);
    result.length = Tundra_u64_to_str_w_buf(num, result.str);

    return result;
}

u64 Tundra_u64_to_str_w_buf(u64 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_DIGITS_FOR_U64];
    
    u64 buf_idx = 0;

    while(num > 0)
    {
        u8 digit = (u8)(num % 10);
        
        buffer[buf_idx++] = (char)('0' + digit);
        
        num /= 10;
    }

    // Walk back the buffer idx by one since it is pointing one past the end
    // of the read in characters.
    buf_idx--;

    // +1 for null terminator
    // result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_DIGITS_FOR_U64 + 1); 

    u64 output_idx = 0;

    while(true)
    {
        output[output_idx++] = buffer[buf_idx];

        if(buf_idx == 0) break;

        buf_idx -= 1;
    }

    output[output_idx] = '\0';

    return output_idx;
}