/**
 * @file IOInterface.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for interacting with the OS for intput and output.
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
#include "tundra/common/TypeDef.h"

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

i64 InTundra_write_bytes(InTundra_IOHandle handle, const void *bytes, 
    i64 num_bytes)
{
    if (bytes == NULL) return -TUNDRA_ERR_BADADDR;
    if(num_bytes == 0) return 0;

    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_WRITE, handle, (i64)bytes, 
        (i64)num_bytes, 0, 0, 0);
}

i64 InTundra_write_u64(InTundra_IOHandle handle, u64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    u64 converted_length = Tundra_u64_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_i64(InTundra_IOHandle handle, i64 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    u64 converted_length = Tundra_i64_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_u32(InTundra_IOHandle handle, u32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    u64 converted_length = Tundra_u32_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_i32(InTundra_IOHandle handle, i32 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    u64 converted_length = Tundra_int_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_u16(InTundra_IOHandle handle, u16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    u64 converted_length = Tundra_u16_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_i16(InTundra_IOHandle handle, i16 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    u64 converted_length = Tundra_i16_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_u8(InTundra_IOHandle handle, u8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    u64 converted_length = Tundra_u8_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_i8(InTundra_IOHandle handle, i8 num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    u64 converted_length = Tundra_i8_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_float(InTundra_IOHandle handle, float num)
{
    char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    u64 converted_length = Tundra_float_to_cstr_buf(num, buffer);

    return InTundra_write_bytes(handle, (const void*)buffer, 
        (i64)converted_length);
}

i64 InTundra_write_formatted(InTundra_IOHandle handle, const char *format, ...)
{
    if(format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = InTundra_vargs_write_formatted(handle, format, args);

    Tundra_varg_end(args);

    return result;
}

i64 InTundra_vargs_write_formatted(InTundra_IOHandle handle, const char *format,
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
            // i64 print_result = Tundra_print_char(c);
            i64 print_result = InTundra_write_bytes(handle, (const void*)&c,
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
            // Tundra_varg_end(args);
            return -TUNDRA_ERR_INVARG;
        }

        switch(c)
        {
            case '%':
            {
                // i64 print_result = Tundra_print_char('%');
                i64 print_result = InTundra_write_bytes(handle, 
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

                i64 print_result = InTundra_write_bytes(handle, 
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

                i64 print_result = InTundra_write_bytes(handle, varg, 
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
                i64 print_result = InTundra_write_u32(handle, (u32)varg);

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
                i64 print_result = InTundra_write_i32(handle, varg);

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

i64 InTundra_read_bytes(InTundra_IOHandle handle, void *output, i64 num_bytes)
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




