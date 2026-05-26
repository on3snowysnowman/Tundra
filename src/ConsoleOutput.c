/**
 * @file ConsoleOutput.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic outputting to the Console.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/ConsoleOutput.h"
#include "tundra/internal/IOInterface.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/ToString.h"

i64 Tundra_stdout_byte(u8 byte) 
{
    return InTundra_write_bytes(TUNDRA_STD_OUT, &byte, 1);
}

i64 Tundra_stdout_char(char c) 
{
    return Tundra_stdout_byte((u8)c);
}

i64 Tundra_stdout_cstr(const char *cstr)
{
    if (cstr == NULL) return -TUNDRA_EFAULT;
    
    u64 length = Tundra_get_str_len(cstr);

    return Tundra_stdout_cstr_w_len(cstr, length);
}

i64 Tundra_stdout_cstr_w_len(const char *cstr, u64 length)
{
    return InTundra_write_bytes(TUNDRA_STD_OUT, cstr, length);
}

i64 Tundra_stdout_u64(u64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    u64 converted_length = Tundra_u64_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_i64(i64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    u64 converted_length = Tundra_i64_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_u32(u32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    u64 converted_length = Tundra_u32_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_int(int num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    u64 converted_length = Tundra_int_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_u16(u16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    u64 converted_length = Tundra_u16_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_i16(i16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    u64 converted_length = Tundra_i16_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_u8(u8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    u64 converted_length = Tundra_u8_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_i8(i8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    u64 converted_length = Tundra_i8_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}

i64 Tundra_stdout_float(float num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    u64 converted_length = Tundra_float_to_cstr_buf(num, buffer);

    return Tundra_stdout_cstr_w_len(buffer, converted_length);
}
