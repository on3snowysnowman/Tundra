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
#include "tundra/common/ErrorDef.h"
#include "tundra/internal/Syscall.h"

/**
 * @brief Returns the cursor position inside an open file. 
 * If the return is negative it is an error code. Otherwise it is the byte 
 * position of the cursor in the file.
 * 
 * @param handle File handle to inspect.
 * 
 * @return i64 Byte position of the cursor in the file if return is non 
 * negative, otherwise it is an error code.
 */
static i64 get_cursor_pos_in_file(const Tundra_File *file)
{
    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 0, 
        TUNDRA_LINUX_SEEKBEHAVIOR_CUR, 0, 0, 0);
}

/**
 * @brief Moves the cursor inside an open file to a specified position.
 * If the return is negative it is an error code. Otherwise it is the byte 
 * position of the cursor in the file.
 * 
 * @param byte_position 
 * 
 * @return i64 
 */
static i64 move_cursor_in_file(Tundra_File *file, 
    i64 byte_position)
{
    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 
        byte_position, TUNDRA_LINUX_SEEKBEHAVIOR_SET, 0, 0, 0);
}

/**
 * @brief Returns the file size of the file with the specified handle. 
 * If the returns is negative, it's an error code. If it is non negative it is
 * the size in bytes of the file.
 * 
 * @param handle File handle to inspect.
 * 
 * @return i64 Number of bytes in the file if return is non negative, otherwise
 * it is an error code. 
 */
static i64 find_file_size(Tundra_File *file)
{
    i64 current_cursor_pos = get_cursor_pos_in_file(file);

    // If error
    if(current_cursor_pos < 0) return current_cursor_pos;

    i64 file_size = InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 
        0, TUNDRA_LINUX_SEEKBEHAVIOR_END, 0, 0, 0);

    if(file_size < 0) return file_size;

    // Restore cursor position to what it was.
    i64 move_result = move_cursor_in_file(file, current_cursor_pos);

    if(move_result < 0) return move_result;

    // Cursor has been successfully restored.

    return file_size;
}

/**
 * @brief Attempts to open a file and returns the handle id.
 * If the return is negative, it's an error code. Otherwise it is
 * the the handle id.
 * 
 * The `path` to the file is a relative path from the current directory.
 * 
 * @param path Path to the file, relative to current directory.
 * @param open_flags Flags for open behavior. Combination of Linux behavioral
 * flags.
 * @param create_privileges Only relevant when `open_flags` contains the CREATE
 * flag, sets the user privileges of a newly created file.
 * 
 * @return InTundra_IOHandle File handle id if the return is non negative, 
 * otherwise it is an error code. 
 */
static InTundra_IOHandle open_file_helper(const char *path, 
    i64 open_flags, i64 create_privileges)
{
    if(path == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_OPENAT, 
        TUNDRA_LINUX_WORKING_DIRECTORY_FD, (i64)path, open_flags, 
        create_privileges, 0, 0);
}

/**
 * @brief Helper for closing a file.
 * 
 * @param file_handle Handle to close.
 * 
 * @return i64 Return result of the close. 
 */
static i64 close_file_helper(InTundra_IOHandle file_handle)
{
    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_CLOSE, (i64)file_handle, 0, 0,
        0, 0, 0);
}

/**
 * @brief Helper for handling behavior after writing to a file. Checks for 
 * errors and increments the file's cursor position, also checking if the file 
 * has expanded in size from the write.
 * 
 * @param file File that was written.
 * @param write_result Return result of the write.
 */
static void write_helper(Tundra_File *file, i64 write_result)
{
    // If result is an error.
    if(write_result < 0) return;

    file->cursor_pos += write_result;

    // We've added bytes to the end of the file.
    if(file->cursor_pos > file->file_byte_size) 
        file->file_byte_size = file->cursor_pos;
}

void Tundra_file_check_openerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to open file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_file_check_closeerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to close file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_file_check_writeerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to write to file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_file_check_readerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to read from file: %s\n", 
        Tundra_err_to_rdbl(result));
}

i64 Tundra_file_open(Tundra_File *file, const char *path, 
    Tundra_FileOpenMode open_mode, Tundra_FileOpenBehavior open_behavior,
    bool create_if_noexist)
{
    if(file == NULL || path == NULL) return -TUNDRA_ERR_BADADDR;

    i64 open_flags = open_mode | open_behavior | 
        (create_if_noexist * TUNDRA_LINUX_FILEOPENFLAG_CREATE);

    i64 open_result = open_file_helper(path, open_flags, 0644);

    // If error
    if(open_result < 0) return open_result;

    file->handle = open_result;

    i64 file_size = find_file_size(file);

    if(file_size < 0) return file_size;

    file->file_byte_size = file_size;

    i64 cursor_pos = get_cursor_pos_in_file(file);

    if(cursor_pos < 0) return cursor_pos;

    file->cursor_pos = cursor_pos;

    return 0;
}

i64 Tundra_file_write_cstr(Tundra_File *file, const char *cstr)
{
    if(file == NULL || cstr == NULL) return -TUNDRA_ERR_BADADDR;

    u64 cstr_len = Tundra_get_cstr_len(cstr);

    const i64 result = InTundra_raw_write_bytes(file->handle, cstr, 
        (i64)cstr_len);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_char(Tundra_File *file, char c)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_bytes(file->handle, (const void *)&c, 1);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_u64(Tundra_File *file, u64 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_u64(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_i64(Tundra_File *file, i64 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_i64(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_u32(Tundra_File *file, u32 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_u32(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_i32(Tundra_File *file, i32 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_i32(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_u16(Tundra_File *file, u16 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_u16(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_i16(Tundra_File *file, i16 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_i16(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_u8(Tundra_File *file, u8 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_u8(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_i8(Tundra_File *file, i8 num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_i8(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_write_float(Tundra_File *file, float num)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    const i64 result = InTundra_raw_write_float(file->handle, num);

    write_helper(file, result);
    return result;
}

i64 Tundra_file_writef(Tundra_File *file, const char *format, ...)
{
    if(file == NULL || format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_file_vargs_writef(file, format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_file_vargs_writef(Tundra_File *file, const char *format, 
    Tundra_VaList args)
{
    // InTundra_vargs_raw_write_fmt checks if format == NULL
    if(file == NULL ) return -TUNDRA_ERR_BADADDR;

    return InTundra_vargs_raw_write_fmt(file->handle, format, args);
}

i64 Tundra_file_read_bytes(Tundra_File *file, void *buffer, u64 num_bytes)
{
    // InTundra_raw_read_bytes checks for buffer == NULL and num bytes == 0
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_raw_read_bytes(file->handle, buffer, (i64)num_bytes);
}

i64 Tundra_file_close(Tundra_File *file)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    i64 close_result = close_file_helper(file->handle);

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
