/**
 * @file ConsoleIO.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic inputting and outputting with the Console.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/ConsoleIO.h"
#include "tundra/internal/IOInterface.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/ToString.h"
#include "tundra/common/VariadicArgs.h"
#include "tundra/common/ErrorDef.h"

i64 Tundra_print_byte(u8 byte) 
{
    return InTundra_write_bytes(TUNDRA_IOHANDLE_STDOUT, &byte, 1);
}

i64 Tundra_print_char(char c) 
{
    return Tundra_print_byte((u8)c);
}

i64 Tundra_print_cstr(const char *cstr)
{
    if (cstr == NULL) return -TUNDRA_ERR_BADADDR;

    u64 length = Tundra_get_cstr_len(cstr);

    return Tundra_print_cstr_w_len(cstr, length);
}

i64 Tundra_print_cstr_w_len(const char *cstr, u64 length)
{
    return InTundra_write_bytes(TUNDRA_IOHANDLE_STDOUT, (const void*)cstr, 
        (i64)length);
}

i64 Tundra_print_u64(u64 num)
{
    return InTundra_write_u64(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i64(i64 num)
{
    return InTundra_write_i64(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u32(u32 num)
{
    return InTundra_write_u32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_int(int num)
{
    return InTundra_write_i32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u16(u16 num)
{
    return InTundra_write_u16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i16(i16 num)
{
    return InTundra_write_i16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u8(u8 num)
{
    return InTundra_write_u8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i8(i8 num)
{
    return InTundra_write_i8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_float(float num)
{
    return InTundra_write_float(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_printf(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_vargs_printf(format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_vargs_printf(const char *format, Tundra_VaList args)
{
    return InTundra_vargs_write_formatted(TUNDRA_IOHANDLE_STDOUT, format, args);
}

#include "tundra/utils/FatalHandler.h"
void Tundra_readin_bytes(void *output, i64 max_bytes, i64 *read_result_output)
{
    TUNDRA_FATAL("Not implemented.\n");
}

char Tundra_readin_char(i64 *read_result_output)
{
    TUNDRA_FATAL("Not implemented.\n");
    return -1;
}
