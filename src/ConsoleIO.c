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
#include "tundra/utils/StringConversion.h"
#include "tundra/common/VariadicArgs.h"
#include "tundra/common/ErrorDef.h"
#include "tundra/internal/IOBuffer.h"
#include "tundra/common/TypeDef.h"

InTundra_InputBuffer stdin_buff;
InTundra_OutputBuffer stdout_buff;

i64 Tundra_print_cstr(const char *cstr)
{
    // Don't need NULL check here, handled by get_cstr_len and following print
    // call.
    
    const u64 cstr_len = Tundra_get_cstr_len(cstr);

    return Tundra_print_cstr_w_len(cstr, cstr_len);
}

i64 Tundra_print_cstr_w_len(const char *cstr, u64 length)
{
    if(cstr == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_OBuff_write_bytes(&stdout_buff, (const u8*)cstr, length);
}

#define PRINT_NUM_IMPL(type) \
i64 Tundra_print_##type(type num) { \
    return InTundra_OBuff_write_##type(&stdout_buff, num); }

PRINT_NUM_IMPL(char)
PRINT_NUM_IMPL(u64)
PRINT_NUM_IMPL(i64)
PRINT_NUM_IMPL(u32)
PRINT_NUM_IMPL(int)
PRINT_NUM_IMPL(u16)
PRINT_NUM_IMPL(i16)
PRINT_NUM_IMPL(u8)
PRINT_NUM_IMPL(i8)
PRINT_NUM_IMPL(float)

i64 Tundra_printf(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_vargs_printf(format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_eprintf(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_vargs_eprintf(format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_vargs_printf(const char *format, Tundra_VaList args)
{
    Tundra_DynamicArrayChar chars = InTundra_vargs_convert_fmt_to_chars(
        format, args);

    i64 result = InTundra_OBuff_write_bytes(&stdout_buff,
        (const u8*)Tundra_DynArrChar_data(&chars), 
        Tundra_DynArrChar_size(&chars));

    Tundra_DynArrChar_free(&chars);

    return result;
}

i64 Tundra_vargs_eprintf(const char *format, Tundra_VaList args)
{
    return InTundra_vargs_raw_writef(TUNDRA_IOHANDLE_ERROUT, format, args);
}

void Tundra_readin_bytes(u8 *output, u64 max_bytes, i64 *read_result_output)
{
    InTundra_IBuff_readin_bytes(&stdin_buff, output, max_bytes,
        read_result_output);
}

#define READIN_NUM_IMPL(type) \
type Tundra_readin_##type(i64 *read_result_output) {\
    return InTundra_IBuff_readin_##type(&stdin_buff, read_result_output); }

READIN_NUM_IMPL(char)
READIN_NUM_IMPL(u8)
READIN_NUM_IMPL(i8)
READIN_NUM_IMPL(u16)
READIN_NUM_IMPL(i16)
READIN_NUM_IMPL(u32)
READIN_NUM_IMPL(int)
READIN_NUM_IMPL(u64)
READIN_NUM_IMPL(i64)
READIN_NUM_IMPL(float)

void Tundra_flush_stdout(void)
{
    InTundra_OBuff_flush(&stdout_buff);
}

void Tundra_clear_stdin(void)
{
    InTundra_IBuff_clear(&stdin_buff);
}

void InTundra_ConIO_init(void)
{
    InTundra_OBuff_init(&stdout_buff, TUNDRA_IOHANDLE_STDOUT);
    InTundra_IBuff_init(&stdin_buff, TUNDRA_IOHANDLE_STDIN);
}

void InTundra_ConIO_shutdown(void)
{
    InTundra_OBuff_flush(&stdout_buff);

    InTundra_OBuff_free(&stdout_buff);
    InTundra_IBuff_free(&stdin_buff);
}
