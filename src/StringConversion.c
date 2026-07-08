/**
 * @file StringConversion.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for converting numbers to C strings and C strings to numbers.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/StringConversion.h"
#include "tundra/common/Core.h"
#include "tundra/common/NumLimits.h"
#include "tundra/utils/Math.h"
#include "tundra/utils/MemUtils.h"

float InTundra_float_rounding_constant = 0.0f;

// Returns number of characters in buffer not including null terminator.
static u64 InTundra_int_to_cstr_helper(u64 num, char *buffer, char *output, 
    bool negative_num)
{
    if(num == 0)
    {
        output[0] = '0';
        output[1] = '\0'; 
        return 1;
    }

    u64 buff_idx = 0;

    while(num > 0)
    {
        u8 digit = (u8)(num % 10);
        
        buffer[buff_idx++] = (char)('0' + digit);
        
        num /= 10;
    }

    // Walk back the buffer idx by one since it is pointing one past the end
    // of the read in characters.
    buff_idx--;

    u64 output_idx = 0;

    if(negative_num)
    {
        output[output_idx++] = '-';
    }

    while(true)
    {
        output[output_idx++] = buffer[buff_idx];

        if(buff_idx == 0) break;

        --buff_idx;
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

    // The float has a fractional part to it. Round it.
    if((u64)num != num) num += InTundra_float_rounding_constant;

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

static u64 str_to_unum_helper(const char *str)
{
    TUNDRA_RT_ASSERT(str != NULL, "Attempted to convert a NULL string to a "
        "number.\n");

    u64 str_len = Tundra_get_cstr_len(str);

    TUNDRA_RT_ASSERT(str_len > 0, "Attempted to convert an empty string to "
        "a number.\n");

    u64 result = 0;

    u8 pow = 0;

    u64 str_idx = str_len - 1;

    while(true)
    {
        char c = str[str_idx];

        TUNDRA_RT_ASSERT(c >= '0' && c <= '9', "Invalid character found when "
            "converting string \"%s\" to number. Invalid char: \"%c\".\n", 
            str, c);

        result += Tundra_uint_pow(10, pow++) * (u64)(c - '0');

        if(str_idx == 0) break;

        --str_idx;
    } 

    return result;
}

static i64 str_to_snum_helper(const char *str)
{
    TUNDRA_RT_ASSERT(str != NULL, "Attempted to convert a NULL string to a "
        "number.\n");

    u64 str_len = Tundra_get_cstr_len(str);

    TUNDRA_RT_ASSERT(str_len > 0, "Attempted to convert an empty string to "
        "a number.\n");

    u64 num_start_idx = 0;

    if(str[0] == '-')
    {
        TUNDRA_RT_ASSERT(str_len > 1, "Attempted to convert invalid string to "
            "number: \"%s\".\n", str);

        num_start_idx = 1;
    }

    u64 result = 0;

    u8 pow = 0;

    u64 str_idx = str_len - 1;

    while(true)
    {
        char c = str[str_idx];

        TUNDRA_RT_ASSERT(c >= '0' && c <= '9', "Invalid character found when "
            "converting string \"%s\" to number. Invalid char: \"%c\".\n", 
            str, c);

        result += Tundra_uint_pow(10, pow++) * (u64)(c - '0');

        if(str_idx == num_start_idx) break;

        --str_idx;
    } 

    if(num_start_idx == 1)
    {
        TUNDRA_RT_ASSERT((i64)result - TUNDRA_INT64_MAX <= 1, "Attempted to convert "
            "a string to an i64 that exceeded the numeric limits of an i64. "
            "String: \"%s\".\n", str);
    }

    return num_start_idx == 1 ? (i64)-result : (i64)result;
}

u8 Tundra_str_to_u8(const char *str)
{
    u64 result = str_to_unum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_UINT8_MAX, "Attempted to convert a "
        "string to an u8 that exceeded the numeric limits of an u8. "
        "String: \"%s\".\n", str)

    return (u8)result;
}

i8 Tundra_str_to_i8(const char *str)
{
    i64 result = str_to_snum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_INT8_MAX && result >= TUNDRA_INT8_MIN,
        "Attempted to convert a string to an i8 that exceeded the numeric "
        "limits of an i8. String: \"%s\".\n", str);

    return (i8)result;
}

u16 Tundra_str_to_u16(const char *str)
{
    u64 result = str_to_unum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_UINT16_MAX,
        "Attempted to convert a string to an u16 that exceeded the numeric "
        "limits of an u16. String: \"%s\".\n", str);

    return (u16)result;
}

i16 Tundra_str_to_i16(const char *str)
{
    i64 result = str_to_snum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_INT16_MAX && result >= TUNDRA_INT16_MIN,
        "Attempted to convert a string to an i16 that exceeded the numeric "
        "limits of an i16. String: \"%s\".\n", str);

    return (i16)result;
}

u32 Tundra_str_to_u32(const char *str)
{
    u64 result = str_to_unum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_UINT32_MAX,
        "Attempted to convert a string to an u32 that exceeded the numeric "
        "limits of an u32. String: \"%s\".\n", str);

    return (u32)result;
}

int Tundra_str_to_int(const char *str)
{
    i64 result = str_to_snum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_INT32_MAX && result >= TUNDRA_INT32_MIN,
        "Attempted to convert a string to an int that exceeded the numeric "
        "limits of an int. String: \"%s\".\n", str);

    return (int)result;
}

u64 Tundra_str_to_u64(const char *str)
{
    u64 result = str_to_unum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_UINT64_MAX,
        "Attempted to convert a string to an u64 that exceeded the numeric "
        "limits of an u64. String: \"%s\".\n", str);

    return (u64)result;
}

i64 Tundra_str_to_i64(const char *str)
{
    i64 result = str_to_snum_helper(str);

    TUNDRA_RT_ASSERT(result <= TUNDRA_INT64_MAX && result >= TUNDRA_INT64_MIN,
        "Attempted to convert a string to an i64 that exceeded the numeric "
        "limits of an i64. String: \"%s\".\n", str);

    return (i64)result;
}

float Tundra_str_to_float(const char *str)
{
    TUNDRA_RT_ASSERT(str != NULL, "Attempted to convert a NULL string to a "
        "number.\n");

    u64 str_len = Tundra_get_cstr_len(str);

    TUNDRA_RT_ASSERT(str_len > 0, "Attempted to convert an empty string to "
        "a number.\n");

    bool is_negative = false;

    u64 str_idx = 0;

    if(str[0] == '-')
    {
        TUNDRA_RT_ASSERT(str_len > 1, "Attempted to convert invalid string to "
            "number: \"%s\".\n", str);

        is_negative = true;
        ++str_idx;
    }

    u64 int_part = 0;
    u64 frac_part = 0;
    u64 frac_divisor = 1;

    // Parse digits before decimal point, if there is one.
    while(str_idx < str_len)
    {
        const char c = str[str_idx];
    
        if(c == '.') break;

        TUNDRA_RT_ASSERT(c >= '0' && c <= '9', "Invalid character found when "
            "converting string \"%s\" to float. Invalid char: \"%c\".\n",
            str, c);
        
        u8 digit = (u8)(c - '0');

        int_part = (int_part * 10) + digit;

        ++str_idx;
    }

    // If we found a decimal.
    if(str_idx < str_len && str[str_idx] == '.')
    {
        ++str_idx; // Skip the decimal.

        // Make sure we don't have a decimal followed by nothing.
        TUNDRA_RT_ASSERT(str_idx < str_len, "Attempted to convert invalid "
            "string to float, found decimal with nothing past it. String: "
            "\"%s\".\n", str);
    }

    u8 num_frac_digits = 0;

    // If we didn't find a decimal this never runs.
    // Add +1 to the precision since we want to grab one more decimal than
    // we can represent to achieve rounding.
    while(str_idx < str_len)
    {
        const char c = str[str_idx++];

        TUNDRA_RT_ASSERT(c >= '0' && c <= '9', "Invalid character found when "
            "converting string \"%s\" to float. Invalid char: \"%c\".\n",
            str, c);
        
        u8 digit = (u8)(c - '0');

        // We've reached one past the decimal point we can represent. We use 
        // this unrepresentable digit to round the float we can represent then
        // stop parsing.
        if(num_frac_digits == TUNDRA_FLOAT_PRECISION)
        {
            if(digit >= 5) frac_part += 1;
            break;
        }

        frac_part = (frac_part * 10) + digit;
        frac_divisor *= 10;

        ++num_frac_digits;
    }

    float result;

    if(frac_part == 0) result = (float)int_part;
    else result = (float)int_part + ((float)frac_part / (float)frac_divisor);

    return is_negative ? -result : result;
}