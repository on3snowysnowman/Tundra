/**
 * @file FileHandling.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing files. Opening and closing as well as writing 
 * and reading.
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/FileHandling.h"
#include "tundra/common/Core.h"
#include "tundra/utils/ToString.h"
#include "tundra/utils/MemUtils.h"


i64 Tundra_file_open(Tundra_File *file, const char *path, 
    Tundra_FileOpenMode open_mode, Tundra_FileOpenBehavior open_behavior,
    bool create_if_noexist)
{
    if(file == NULL || path == NULL) return -TUNDRA_EFAULT;

    i64 open_flags = open_mode | open_behavior | 
        (create_if_noexist * TUNDRA_LINUX_FILEOPENFLAG_CREATE);

    i64 open_result = InTundra_open_file(path, open_flags, 0644);

    // If error
    if(open_result < 0) return open_result;

    file->handle = open_result;

    i64 file_size = InTundra_get_file_size(file->handle);

    if(file_size < 0) return file_size;

    file->file_byte_size = file_size;

    i64 cursor_pos = InTundra_get_cursor_pos_in_file(file->handle);

    if(cursor_pos < 0) return cursor_pos;

    file->cursor_pos = cursor_pos;

    return 0;
}

i64 write_helper(Tundra_File *file, i64 write_result)
{
    if(write_result < 0) return write_result;

    file->cursor_pos += write_result;

    // We've added bytes to the end of the file.
    if(file->cursor_pos > file->file_byte_size) 
        file->file_byte_size = file->cursor_pos;

    return write_result;
}

i64 Tundra_file_write_cstr(Tundra_File *file, const char *cstr)
{
    if(file == NULL || cstr == NULL) return -TUNDRA_EFAULT;

    u64 cstr_len = Tundra_get_cstr_len(cstr);

    const i64 result = InTundra_write_bytes(file->handle, cstr, 
        (i64)cstr_len);

    return write_helper(file, result);

    // // If error
    // if(num_written_bytes < 0) return num_written_bytes;

    // file->cursor_pos += num_written_bytes;

    // // We've added bytes to the end of the file.
    // if(file->cursor_pos > file->file_byte_size) 
    //     file->file_byte_size = file->cursor_pos;

    // return num_written_bytes;
}

i64 Tundra_file_write_char(Tundra_File *file, char c)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_bytes(file->handle, (const void *)&c, 1);

    return write_helper(file, result);
}

i64 Tundra_file_write_u64(Tundra_File *file, u64 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_u64(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_i64(Tundra_File *file, i64 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_i64(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_u32(Tundra_File *file, u32 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_u32(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_i32(Tundra_File *file, i32 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_i32(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_u16(Tundra_File *file, u16 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_u16(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_i16(Tundra_File *file, i16 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_i16(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_u8(Tundra_File *file, u8 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_u8(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_i8(Tundra_File *file, i8 num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_i8(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_write_float(Tundra_File *file, float num)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    const i64 result = InTundra_write_float(file->handle, num);

    return write_helper(file, result);
}

i64 Tundra_file_writef(Tundra_File *file, const char *format, ...)
{
    if(file == NULL || format == NULL) return -TUNDRA_EFAULT;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_file_vargs_writef(file, format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_file_vargs_writef(Tundra_File *file, const char *format, 
    Tundra_VaList args)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    return InTundra_vargs_write_formatted(file->handle, format, args);
}

i64 Tundra_file_close(Tundra_File *file)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    i64 close_result = InTundra_close_file(file->handle);

    if(close_result < 0) return close_result;

    file->handle = TUNDRA_IOHANDLE_INVALID;
    file->file_byte_size = 0;
    file->cursor_pos = 0;

    return 0;
}

u64 Tundra_file_get_size(const Tundra_File *file)
{
    if(file == NULL) return 0;

    return (u64)file->file_byte_size;
}
