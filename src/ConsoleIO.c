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


i64 Tundra_print_byte(u8 byte) 
{
    // return InTundra_raw_write_bytes(TUNDRA_IOHANDLE_STDOUT, &byte, 1);
    return InTundra_OBuff_write(&stdout_buff, &byte, 1);
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
}

i64 Tundra_print_cstr_w_len(const char *cstr, u64 length)
{
    if(cstr == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_OBuff_write(&stdout_buff, (const u8*)cstr, length);
}

i64 Tundra_print_u64(u64 num)
{
    // +1 for null terminator
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    u64 converted_len = Tundra_u64_to_cstr_buf(num, buffer);

    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_i64(i64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    u64 converted_len = Tundra_i64_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_u32(u32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    u64 converted_len = Tundra_u32_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_int(int num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    u64 converted_len = Tundra_int_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_u16(u16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    u64 converted_len = Tundra_u16_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_i16(i16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    u64 converted_len = Tundra_i16_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_u8(u8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    u64 converted_len = Tundra_u8_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_i8(i8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    u64 converted_len = Tundra_i8_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
}

i64 Tundra_print_float(float num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    u64 converted_len = Tundra_float_to_cstr_buf(num, buffer);
    
    return InTundra_OBuff_write(&stdout_buff, (const u8*)buffer, 
        converted_len);
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

i64 Tundra_eprintf(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = InTundra_vargs_raw_writef(TUNDRA_IOHANDLE_ERROUT, format,
        args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_vargs_printf(const char *format, Tundra_VaList args)
{
    Tundra_DynamicArrayChar chars = InTundra_vargs_convert_fmt_to_chars(
        format, args);

    i64 result = InTundra_OBuff_write(&stdout_buff,
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
    i64 result = InTundra_IBuff_read(&stdin_buff, output, max_bytes);

    if(read_result_output != NULL) *read_result_output = result;
}

char Tundra_readin_char(i64 *read_result_output)
{
    char c = 0;
    
    i64 result = InTundra_IBuff_read(&stdin_buff, (u8*)&c, 1);

    if(read_result_output != NULL) *read_result_output = result;

    return c;
}

#define READIN_IMPL(type, BUFF_SIZE) \
type Tundra_readin_##type(i64 *read_result_output) \
{ \
    /**  +1 for '\0' */ \
    char buffer[(BUFF_SIZE) + 1]; \
    u64 buff_idx = 0; \
    while(true) \
    { \
        const i64 result = InTundra_IBuff_peek(&stdin_buff); \
        if(result < 0) \
        { \
            if(read_result_output != NULL) *read_result_output = result; \
            return 0; \
        } \
        const char c = (char)result; \
        if(c == '\n' || c == ' ') break; \
        if(buff_idx == (BUFF_SIZE)) \
        { \
            TUNDRA_FATAL("Attempted to convert a read in " #type \
                " with too many digits.\n"); \
            return 0; \
        } \
        InTundra_IBuff_discard(&stdin_buff, 1); \
        buffer[buff_idx] = c; \
        ++buff_idx; \
    } \
    TUNDRA_RT_ASSERT(buff_idx != 0, "Attempted to convert read in whitespace " \
        "to " #type); \
    buffer[buff_idx] = '\0'; \
    if(read_result_output != NULL) *read_result_output = (i64)buff_idx - 1; \
    return Tundra_str_to_##type(buffer); \
}

READIN_IMPL(u8, TUNDRA_MAX_CHARS_TO_REPRESENT_U8)
READIN_IMPL(i8, TUNDRA_MAX_CHARS_TO_REPRESENT_I8)
READIN_IMPL(u16, TUNDRA_MAX_CHARS_TO_REPRESENT_U16)
READIN_IMPL(i16, TUNDRA_MAX_CHARS_TO_REPRESENT_I16)
READIN_IMPL(u32, TUNDRA_MAX_CHARS_TO_REPRESENT_U32)
READIN_IMPL(int, TUNDRA_MAX_CHARS_TO_REPRESENT_I32)
READIN_IMPL(u64, TUNDRA_MAX_CHARS_TO_REPRESENT_U64)
READIN_IMPL(i64, TUNDRA_MAX_CHARS_TO_REPRESENT_I64)

float Tundra_readin_float(i64 *read_result_output)
{
    // +1 for '\0'
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];

    u64 buff_idx = 0;

    bool decimal_found = false;

    // Read in digits before the decimal place if it exists.
    while(true)
    {
        const i64 result = InTundra_IBuff_peek(&stdin_buff);

        if(result < 0)
        {
            if(read_result_output != NULL) *read_result_output = result;
            return 0.0f;
        }

        const char c = (char)result;

        if(c == '\n' || c == ' ') break;
        else if(c == '.')
        {
            if(decimal_found != false)
            {
                TUNDRA_RT_ASSERT(decimal_found != true, "Attempted to convert "
                    "a read in float with two decimal points.\n");
            }
            
            decimal_found = true;
        }

        TUNDRA_RT_ASSERT(buff_idx != TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1,
            "Attempted to convert a read in float with too many digits.\n");

        InTundra_IBuff_discard(&stdin_buff, 1);
        buffer[buff_idx] = c;

        ++buff_idx;
    }

    TUNDRA_RT_ASSERT(buff_idx != 0, "Attempted to convert read in whitespace " 
        "to float"); 

    buffer[buff_idx] = '\0';
    if(read_result_output != NULL) *read_result_output = (i64)buff_idx - 1;

    return Tundra_str_to_float(buffer);
}

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
