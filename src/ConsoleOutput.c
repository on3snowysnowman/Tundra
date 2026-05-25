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
    // Tundra_ToStrResult num_to_str = Tundra_u64_to_str(num);

    char buffer[TUNDRA_MAX_DIGITS_FOR_U64 + 1];
    u64 converted_length = Tundra_u64_to_str_w_buf(num, buffer);

    i64 result = Tundra_stdout_cstr_w_len(buffer, converted_length);

    // Tundra_free_mem(num_to_str.str);

    return result;
}
