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
#include "tundra/common/VariadicArgs.h"

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
    if (cstr == NULL) return -TUNDRA_EFAULT;

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
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1];
    // u64 converted_length = Tundra_u64_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
}

i64 Tundra_print_i64(i64 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1];
    // u64 converted_length = Tundra_i64_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_i64(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u32(u32 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1];
    // u64 converted_length = Tundra_u32_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_u32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_int(int num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1];
    // u64 converted_length = Tundra_int_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_i32(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u16(u16 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1];
    // u64 converted_length = Tundra_u16_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_u16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i16(i16 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I16 + 1];
    // u64 converted_length = Tundra_i16_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_i16(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_u8(u8 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1];
    // u64 converted_length = Tundra_u8_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_u8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_i8(i8 num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1];
    // u64 converted_length = Tundra_i8_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_i8(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_print_float(float num)
{
    // char buffer[TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT + 1];
    // u64 converted_length = Tundra_float_to_cstr_buf(num, buffer);

    // return Tundra_print_cstr_w_len(buffer, converted_length);
    return InTundra_write_float(TUNDRA_IOHANDLE_STDOUT, num);
}

i64 Tundra_printf(const char *format, ...)
{
    if(format == NULL) return -TUNDRA_EFAULT;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_vargs_printf(format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_vargs_printf(const char *format, Tundra_VaList args)
{
    return InTundra_write_formatted(TUNDRA_IOHANDLE_STDOUT, format, args);
    // u64 fmt_idx = 0;
    
    // char c = format[fmt_idx++];

    // i64 bytes_written = 0;

    // while(c != '\0')
    // {
    //     if(c != '%') 
    //     {
    //         i64 print_result = Tundra_print_char(c);

    //         if(print_result < 0) 
    //         {
    //             // Tundra_varg_end(args);
    //             return print_result;
    //         }
            
    //         bytes_written += print_result;

    //         c = format[fmt_idx++];
    //         continue;
    //     }

    //     c = format[fmt_idx++];
    //     if(c == '\0') 
    //     {
    //         // Tundra_varg_end(args);
    //         return -TUNDRA_EINVAL;
    //     }

    //     switch(c)
    //     {
    //         case '%':
    //         {
    //             i64 print_result = Tundra_print_char('%');
                
    //             if(print_result < 0) 
    //             {
    //                 // Tundra_varg_end(args);
    //                 return print_result;
    //             }
                
    //             bytes_written += print_result;
                
    //             break;
    //         }

    //         case 'c':
    //         {
    //             int varg = Tundra_varg_arg(args, int);

    //             i64 print_result = Tundra_print_char((char)varg);

    //             if(print_result < 0) 
    //             {
    //                 // Tundra_varg_end(args);
    //                 return print_result;
    //             }
                
    //             bytes_written += print_result;
    //             break;
    //         }

    //         case 's':
    //         {
    //             const char *varg = Tundra_varg_arg(args, const char *);
    //             i64 print_result = Tundra_print_cstr(varg);

    //             if(print_result < 0) 
    //             {
    //                 // Tundra_varg_end(args);
    //                 return print_result;
    //             }
                
    //             bytes_written += print_result;
    //             break;
    //         }

    //         case 'u':
    //         {
    //             unsigned int varg = Tundra_varg_arg(args, unsigned int);
    //             i64 print_result = Tundra_print_u32((u32)varg);

    //             if(print_result < 0) 
    //             {
    //                 // Tundra_varg_end(args);
    //                 return print_result;
    //             }
                
    //             bytes_written += print_result;
    //             break;
    //         }

    //         case 'd':
    //         {
    //             int varg = Tundra_varg_arg(args, int);
    //             i64 print_result = Tundra_print_int(varg);

    //             if(print_result < 0) 
    //             {
    //                 // Tundra_varg_end(args);
    //                 return print_result;
    //             }
                
    //             bytes_written += print_result;
    //             break;
    //         }

    //         default: 
            
    //             // Tundra_varg_end(args);
    //             return -TUNDRA_EINVAL;
    //     }

    //     c = format[fmt_idx++];
    // }

    // return bytes_written;
}
