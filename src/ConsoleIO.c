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
#include "tundra/internal/IOBuffer.h"

InTundra_IOBuffer stdin_buff;
InTundra_IOBuffer stdout_buff;
InTundra_IOBuffer stderr_buff;


i64 Tundra_print_byte(u8 byte) 
{
    // return InTundra_raw_write_bytes(TUNDRA_IOHANDLE_STDOUT, &byte, 1);
    return InTundra_IOBuff_write_bytes(&stdout_buff, &byte, 1);
}

i64 Tundra_print_char(char c) 
{
    return Tundra_print_byte((u8)c);
}

i64 Tundra_print_cstr(const char *cstr)
{
    // Don't need NULL check here, handled by get_cstr_len and following print
    // call.

    const u64 cstr_len = Tundra_get_cstr_len(cstr);

    return Tundra_print_cstr_w_len(cstr, cstr_len);

    // bool found_newline = get_cstr_len_check_newline(cstr, &cstr_len);

    // return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)cstr, cstr_len);

    // if(found_newline) InTundra_IOBuff_flush(&stdout_buff);
    // return Tundra_print_cstr_w_len(cstr, length);
}

i64 Tundra_print_cstr_w_len(const char *cstr, u64 length)
{
    if(cstr == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)cstr, length);
}

i64 Tundra_print_u64(u64 num)
{
    // +1 for null terminator
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    u64 converted_len = Tundra_u64_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_u64(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i64(i64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    u64 converted_len = Tundra_i64_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_i64(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u32(u32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    u64 converted_len = Tundra_u32_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_u32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_int(int num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    u64 converted_len = Tundra_int_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_i32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u16(u16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    u64 converted_len = Tundra_u16_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_u16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i16(i16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    u64 converted_len = Tundra_i16_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_i16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u8(u8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    u64 converted_len = Tundra_u8_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_u8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i8(i8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    u64 converted_len = Tundra_i8_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_i8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_float(float num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    u64 converted_len = Tundra_float_to_cstr_buf(num, buffer);

    return InTundra_IOBuff_write_bytes(&stdout_buff, (const u8*)buffer, 
        converted_len);

    // return InTundra_raw_write_float(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_fmt(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_vargs_print_fmt(format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_errprint_fmt(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = InTundra_vargs_raw_write_fmt(TUNDRA_IOHANDLE_ERROUT, format,
        args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_vargs_print_fmt(const char *format, Tundra_VaList args)
{
    Tundra_DynamicArrayChar chars = InTundra_convert_vargs_to_chars(
        format, args);

    i64 result = InTundra_IOBuff_write_bytes(&stdout_buff, 
        (const u8*)Tundra_DynArrChar_data(&chars), 
        Tundra_DynArrChar_size(&chars));

    Tundra_DynArrChar_free(&chars);

    return result;

    // return InTundra_vargs_raw_write_fmt(TUNDRA_IOHANDLE_STDOUT, format, args);
}

i64 Tundra_vargs_errprint_fmt(const char *format, Tundra_VaList args)
{
    return InTundra_vargs_raw_write_fmt(TUNDRA_IOHANDLE_ERROUT, format, args);
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

void Tundra_flush_stdout(void)
{
    InTundra_IOBuff_flush(&stdout_buff);
}

void InTundra_ConIO_init(void)
{
    InTundra_IOBuff_init(&stdin_buff, TUNDRA_IOHANDLE_STDIN);
    InTundra_IOBuff_init(&stdout_buff, TUNDRA_IOHANDLE_STDOUT);
    InTundra_IOBuff_init(&stderr_buff, TUNDRA_IOHANDLE_ERROUT);
}

void InTundra_ConIO_shutdown(void)
{
    InTundra_IOBuff_flush(&stdout_buff);

    InTundra_IOBuff_free(&stdin_buff);
    InTundra_IOBuff_free(&stdout_buff);
    InTundra_IOBuff_free(&stderr_buff);
}
