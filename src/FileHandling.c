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

i64 Tundra_file_write_cstr(Tundra_File *file, const char *cstr)
{
    if(file == NULL || cstr == NULL) return -TUNDRA_EFAULT;

    u64 cstr_len = Tundra_get_str_len(cstr);

    i64 num_written_bytes = InTundra_write_bytes(file->handle, cstr, 
        (i64)cstr_len);

    // If error
    if(num_written_bytes < 0) return num_written_bytes;

    file->cursor_pos += num_written_bytes;

    // We've added bytes to the end of the file.
    if(file->cursor_pos > file->file_byte_size) 
        file->file_byte_size = file->cursor_pos;

    return num_written_bytes;
}

i64 Tundra_file_write_char(Tundra_File *file, char c)
{
    if(file == NULL) return -TUNDRA_EFAULT;

    

    return -1;
}

i64 Tundra_file_write_u64(Tundra_File *file, u64 num)
{
    return -1;
}

i64 Tundra_file_write_i64(Tundra_File *file, i64 num)
{
    return -1;
}

i64 Tundra_file_write_u32(Tundra_File *file, u32 num)
{
    return -1;
}

i64 Tundra_file_write_i32(Tundra_File *file, i32 num)
{
    return -1;
}

i64 Tundra_file_write_u16(Tundra_File *file, u16 num)
{
    return -1;
}

i64 Tundra_file_write_i16(Tundra_File *file, i16 num)
{
    return -1;
}

i64 Tundra_file_write_u8(Tundra_File *file, u8 num)
{
    return -1;
}

i64 Tundra_file_write_i8(Tundra_File *file, i8 num)
{
    return -1;
}

i64 Tundra_file_write_float(Tundra_File *file, float num)
{
    return -1;
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
