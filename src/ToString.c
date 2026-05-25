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


u64 InTundra_int_to_str_helper(u64 num, char *buffer, char *output, 
    bool negative_num)
{
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

    if(negative_num)
    {
        output[output_idx++] = '-';
    }

    while(true)
    {
        output[output_idx++] = buffer[buf_idx];

        if(buf_idx == 0) break;

        buf_idx -= 1;
    }

    output[output_idx] = '\0';

    return output_idx;
}

Tundra_ToStrResult Tundra_u64_to_str(u64 num)
{
    Tundra_ToStrResult result;

    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1);
    result.length = Tundra_u64_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_i64_to_str(i64 num)
{
    Tundra_ToStrResult result;
    
    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1);
    result.length = Tundra_i64_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_u32_to_str(u32 num)
{
    Tundra_ToStrResult result;

    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1);
    result.length = Tundra_u32_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_int_to_str(int num)
{
    Tundra_ToStrResult result;
    
    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1);
    result.length = Tundra_int_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_u16_to_str(u16 num)
{
    Tundra_ToStrResult result;

    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1);
    result.length = Tundra_u16_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_i16_to_str(i16 num)
{
    Tundra_ToStrResult result;
    
    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1);
    result.length = Tundra_i16_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_u8_to_str(u8 num)
{
    Tundra_ToStrResult result;

    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1);
    result.length = Tundra_u8_to_str_buf(num, result.str);

    return result;
}

Tundra_ToStrResult Tundra_i8_to_str(i8 num)
{
    Tundra_ToStrResult result;
    
    if(num == 0)
    {
        result.str = (char*)Tundra_alloc_mem(2);
        result.str[0] = '0';
        result.str[1] = '\0';
        result.length = 1;
        return result;
    }

    // +1 for null terminator.
    result.str = (char*)Tundra_alloc_mem(TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1);
    result.length = Tundra_i8_to_str_buf(num, result.str);

    return result;
}


u64 Tundra_u64_to_str_buf(u64 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64];
    
    return InTundra_int_to_str_helper(num, buffer, output, false);
}

u64 Tundra_i64_to_str_buf(i64 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64];
    
    // Funky negation here due to i64 minimum not fitting inside i64 when 
    // negated. 
    if(num < 0) return InTundra_int_to_str_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_u32_to_str_buf(u32 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32];
    
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_int_to_str_buf(int num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32];
    
    // Funky negation here due to i64 minimum not fitting inside i64 when 
    // negated. 
    if(num < 0) return InTundra_int_to_str_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_u16_to_str_buf(u16 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16];
    
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_i16_to_str_buf(i16 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16];
    
    // Funky negation here due to i64 minimum not fitting inside i64 when 
    // negated. 
    if(num < 0) return InTundra_int_to_str_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_u8_to_str_buf(u8 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8];
    
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}

u64 Tundra_i8_to_str_buf(i8 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8];
    
    // Funky negation here due to i64 minimum not fitting inside i64 when 
    // negated. 
    if(num < 0) return InTundra_int_to_str_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_str_helper((u64)num, buffer, output, false);
}
