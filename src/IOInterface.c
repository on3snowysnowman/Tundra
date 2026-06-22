/**
 * @file IOInterface.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for reading and writing to specified handles for primitive 
 * input and output.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/IOInterface.h"
#include "tundra/internal/Syscall.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/common/Core.h"
#include "tundra/utils/ToString.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/common/TypeDef.h"
#include "tundra/common/Core.h"

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

i64 InTundra_raw_write_bytes(InTundra_IOHandle handle, const void *bytes, 
    i64 num_bytes)
{
    if (bytes == NULL) return -TUNDRA_ERR_BADADDR;
    if(num_bytes == 0) return 0;

    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_WRITE, handle, (i64)bytes, 
        (i64)num_bytes, 0, 0, 0);
}

i64 InTundra_raw_write_u64(InTundra_IOHandle handle, u64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    u64 converted_length = Tundra_u64_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_i64(InTundra_IOHandle handle, i64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    u64 converted_length = Tundra_i64_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_u32(InTundra_IOHandle handle, u32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    u64 converted_length = Tundra_u32_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_i32(InTundra_IOHandle handle, i32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    u64 converted_length = Tundra_int_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_u16(InTundra_IOHandle handle, u16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    u64 converted_length = Tundra_u16_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_i16(InTundra_IOHandle handle, i16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    u64 converted_length = Tundra_i16_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_u8(InTundra_IOHandle handle, u8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    u64 converted_length = Tundra_u8_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_i8(InTundra_IOHandle handle, i8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    u64 converted_length = Tundra_i8_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_raw_write_float(InTundra_IOHandle handle, float num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    u64 converted_length = Tundra_float_to_cstr_buf(num, buffer);

    return InTundra_raw_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

static void add_cstr_to_arr(Tundra_DynamicArrayChar *arr, const char *str)
{
    u64 idx = 0;

    char c = str[idx++];

    while(c != '\0')
    {
        Tundra_DynArrChar_add_val(arr, c);
        c = str[idx++];
    }
}

static void process_formatted_char(const char *format,
    Tundra_DynamicArrayChar *chars, u64 *fmt_idx, Tundra_VaList args)
{
    char c = format[(*fmt_idx)++];

    switch(c)
    {
        case '%':

            Tundra_DynArrChar_add_val(chars, '%');
            return;

        case 'c':
        {
            int varg = Tundra_varg_arg(args, int);

            Tundra_DynArrChar_add_val(chars, (char)varg);
            break;
        }

        case 's':
        {
            const char *varg = Tundra_varg_arg(args, const char *);

            add_cstr_to_arr(chars, varg);
            break;
        }

        case 'u':
        {
            unsigned int varg = Tundra_varg_arg(args, unsigned int);

            Tundra_CStr converted_int = Tundra_u32_to_cstr(varg);

            Tundra_DynArrChar_add_mult_copy(chars, converted_int.str, 
                converted_int.length);

            Tundra_free_mem(converted_int.str);
            break;
        }

        case 'd':
        {
            int varg = Tundra_varg_arg(args, int);

            Tundra_CStr converted_int = Tundra_int_to_cstr(varg);

            Tundra_DynArrChar_add_mult_copy(chars, converted_int.str,
                converted_int.length);

            Tundra_free_mem(converted_int.str);
            break;
        }

        case 'l':
        {
            // Check if this is a long long 

            c = format[(*fmt_idx)++];

            bool is_unsigned = false;

            if(c == 'u')
            {
                is_unsigned = true;
            }

            else if(c != 'd')
            {
                TUNDRA_FATAL("Invalid formatted character in format string: "
                    "%s\n", format);
                Tundra_exit(1);
            }

            Tundra_CStr converted_int;

            if(is_unsigned)
            {
                u64 varg = Tundra_varg_arg(args, u64);

                converted_int = Tundra_u64_to_cstr(varg);
            }

            else
            {
                i64 varg = Tundra_varg_arg(args, i64);

                converted_int = Tundra_i64_to_cstr(varg);
            }

            Tundra_DynArrChar_add_mult_copy(chars, converted_int.str,  
                converted_int.length);

            Tundra_free_mem(converted_int.str);
            break;
        }

        default:

            TUNDRA_FATAL("Invalid formatted character in format string: %s\n",
                format);
            Tundra_exit(1);
    }
}

Tundra_DynamicArrayChar InTundra_convert_fmt_to_chars(const char *format,
    ...)
{
    Tundra_VaList args;
    Tundra_varg_start(args, format);

    return InTundra_convert_vargs_to_chars(format, args);

    Tundra_varg_end(args);
}

Tundra_DynamicArrayChar InTundra_convert_vargs_to_chars(const char *format,
    Tundra_VaList args)
{
    Tundra_DynamicArrayChar chars;
    Tundra_DynArrChar_init(&chars);

    if(format == NULL) return chars;

    u64 fmt_idx = 0;

    while(true)
    {
        char c = format[fmt_idx++];

        if(c == '\0') return chars;

        if(c != '%')
        {
            Tundra_DynArrChar_add_val(&chars, c);
            continue;
        }

        // Character is a '%', which is the special format character. Process it
    
        process_formatted_char(format, &chars, &fmt_idx, args);
    }
}

i64 InTundra_raw_write_fmt(InTundra_IOHandle handle, const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = InTundra_vargs_raw_write_fmt(handle, format, args);

    Tundra_varg_end(args);

    return result;
}

i64 InTundra_vargs_raw_write_fmt(InTundra_IOHandle handle, const char *format,
    Tundra_VaList args)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    u64 fmt_idx = 0;
    
    char c = format[fmt_idx++];

    i64 bytes_written = 0;

    while(c != '\0')
    {
        if(c != '%') 
        {
            i64 print_result = InTundra_raw_write_bytes(handle, (const void*)&c,
                1);

            if(print_result < 0) 
            {
                // Tundra_varg_end(args);
                return print_result;
            }
            
            bytes_written += print_result;

            c = format[fmt_idx++];
            continue;
        }

        c = format[fmt_idx++];
        if(c == '\0') 
        {
            return -TUNDRA_ERR_INVARG;
        }

        switch(c)
        {
            case '%':
            {
                // i64 print_result = Tundra_print_char('%');
                i64 print_result = InTundra_raw_write_bytes(handle, 
                    (const void*)'%', 1);
                
                if(print_result < 0) 
                {
                    // Tundra_varg_end(args);
                    return print_result;
                }
                
                bytes_written += print_result;
                
                break;
            }

            case 'c':
            {
                int varg = Tundra_varg_arg(args, int);

                // i64 print_result = Tundra_print_char((char)varg);

                i64 print_result = InTundra_raw_write_bytes(handle, 
                    (const void*)&varg, 1);

                if(print_result < 0) 
                {
                    // Tundra_varg_end(args);
                    return print_result;
                }
                
                bytes_written += print_result;
                break;
            }

            case 's':
            {
                const char *varg = Tundra_varg_arg(args, const char *);
                // i64 print_result = Tundra_print_cstr(varg);
                u64 cstr_len = Tundra_get_cstr_len(varg);

                i64 print_result = InTundra_raw_write_bytes(handle, varg, 
                    (i64)cstr_len);

                if(print_result < 0) 
                {
                    // Tundra_varg_end(args);
                    return print_result;
                }
                
                bytes_written += print_result;
                break;
            }

            case 'u':
            {
                unsigned int varg = Tundra_varg_arg(args, unsigned int);
                // i64 print_result = Tundra_print_u32((u32)varg);
                i64 print_result = InTundra_raw_write_u32(handle, (u32)varg);

                if(print_result < 0) 
                {
                    // Tundra_varg_end(args);
                    return print_result;
                }
                
                bytes_written += print_result;
                break;
            }

            case 'd':
            {
                int varg = Tundra_varg_arg(args, int);
                // i64 print_result = Tundra_print_int(varg);
                i64 print_result = InTundra_raw_write_i32(handle, varg);

                if(print_result < 0) 
                {
                    // Tundra_varg_end(args);
                    return print_result;
                }
                
                bytes_written += print_result;
                break;
            }

            default: 
            
                // Tundra_varg_end(args);
                return -TUNDRA_ERR_INVARG;
        }

        c = format[fmt_idx++];
    }

    return bytes_written;
}

i64 InTundra_raw_read_bytes(InTundra_IOHandle handle, void *output, i64 num_bytes)
{
    if(output == NULL) return -TUNDRA_ERR_BADADDR;
    if(num_bytes == 0) return 0;

    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_READ, handle, (i64)output,
        (i64)num_bytes, 0, 0, 0);
}

#else // TUNDRA_SYS_x86_64
#error Not Implemented
#endif // TUNDRA_SYS_x86_64

#else // TUNDRA_PLATFORM_LINUX
#error Not Implemented
#endif // TUNDRA_PLATFORM_LINUX
