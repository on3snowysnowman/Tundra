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


u64 InTundra_int_to_cstr_helper(u64 num, char *buffer, char *output, 
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

Tundra_CStr Tundra_u64_to_cstr(u64 num)
{
    Tundra_CStr result;

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
    result.length = Tundra_u64_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_i64_to_cstr(i64 num)
{
    Tundra_CStr result;
    
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
    result.length = Tundra_i64_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_u32_to_cstr(u32 num)
{
    Tundra_CStr result;

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
    result.length = Tundra_u32_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_int_to_cstr(int num)
{
    Tundra_CStr result;
    
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
    result.length = Tundra_int_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_u16_to_cstr(u16 num)
{
    Tundra_CStr result;

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
    result.length = Tundra_u16_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_i16_to_cstr(i16 num)
{
    Tundra_CStr result;
    
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
    result.length = Tundra_i16_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_u8_to_cstr(u8 num)
{
    Tundra_CStr result;

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
    result.length = Tundra_u8_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_i8_to_cstr(i8 num)
{
    Tundra_CStr result;
    
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
    result.length = Tundra_i8_to_cstr_buf(num, result.str);

    return result;
}

Tundra_CStr Tundra_float_to_cstr(float num)
{
    Tundra_CStr result;

    if(num == 0)
    {
        // 2 is for the "0." then the precision counts the number of zeros after
        // that.
        result.length = 2 + TUNDRA_FLOAT_PRECISION;

        result.str = (char*)Tundra_alloc_mem(result.length + 1);

        u64 result_idx = 0;
        result.str[result_idx++] = '0';
        result.str[result_idx++] = '.';
        for(u8 i = 0; i < TUNDRA_FLOAT_PRECISION; ++i)
            result.str[result_idx++] = '0';
        result.str[result_idx] = '\0';
        return result;
    }

    // +1 for null terminator 
    result.str = (char*)Tundra_alloc_mem(
        TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1);    
    result.length = Tundra_float_to_cstr_buf(num, result.str);

    return result;
}


u64 Tundra_u64_to_cstr_buf(u64 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64];
    
    return InTundra_int_to_cstr_helper(num, buffer, output, false);
}

u64 Tundra_i64_to_cstr_buf(i64 num, char *output)
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
    if(num < 0) return InTundra_int_to_cstr_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_u32_to_cstr_buf(u32 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32];
    
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_int_to_cstr_buf(int num, char *output)
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
    if(num < 0) return InTundra_int_to_cstr_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_u16_to_cstr_buf(u16 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16];
    
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_i16_to_cstr_buf(i16 num, char *output)
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
    if(num < 0) return InTundra_int_to_cstr_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_u8_to_cstr_buf(u8 num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0';
        return 1;
    }

    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8];
    
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_i8_to_cstr_buf(i8 num, char *output)
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
    if(num < 0) return InTundra_int_to_cstr_helper((u64)(-(num + 1)) + 1, buffer, 
        output, true);
    return InTundra_int_to_cstr_helper((u64)num, buffer, output, false);
}

u64 Tundra_float_to_cstr_buf(float num, char *output)
{
    if(output == NULL) return 0;

    if(num == 0)
    {
        u64 output_idx = 0;
        output[output_idx++] = '0';
        output[output_idx++] = '.';
        for(u8 i = 0; i < TUNDRA_FLOAT_PRECISION; ++i)
            output[output_idx++] = '0';
        output[output_idx] = '\0';
        return output_idx;
    }

    bool is_negative = num < 0.0f;

    if(is_negative) num = -num;

    num += INTUNDRA_FLOAT_ROUNDING_CONSTANT;


    // Integer part is guaranteed to be non negative.
    u64 integer_part = (u64)num;
    float fraction_part = num - (float)integer_part;
    
    // +1 for null terminator
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + TUNDRA_FLOAT_PRECISION + 
        1];

    // Use i64 conversion on the integer part. The returned length is where the
    // fractional part starts.
    u64 output_idx = InTundra_int_to_cstr_helper(integer_part, buffer, output, 
        is_negative);

    output[output_idx++] = '.';

    for(u8 i = 0; i < TUNDRA_FLOAT_PRECISION; ++i)
    {
        fraction_part *= 10;
        u8 digit = (u8)fraction_part;
        fraction_part -= (float)digit;

        output[output_idx++] = (char)('0' + digit);
    }

    output[output_idx] = '\0';

    return output_idx;
}

