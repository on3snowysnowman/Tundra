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
#include "tundra/utils/StringConversion.h"
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
 * @return `i64` Byte position of the cursor in the file if return is non 
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
 * @return `i64` 
 */
// static i64 move_cursor_in_file(Tundra_File *file, 
//     i64 byte_position)
// {
//     return InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 
//         byte_position, TUNDRA_LINUX_SEEKBEHAVIOR_SET, 0, 0, 0);
// }

/**
 * @brief Returns the file size of the file with the specified handle. 
 * If the returns is negative, it's an error code. If it is non negative it is
 * the size in bytes of the file.
 * 
 * @param handle File handle to inspect.
 * 
 * @return `i64` Number of bytes in the file if return is non negative, otherwise
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
    // i64 move_result = move_cursor_in_file(file, current_cursor_pos);
    i64 move_result = InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 
        current_cursor_pos, TUNDRA_LINUX_SEEKBEHAVIOR_SET, 0, 0, 0);

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
 * @return `i64` Return result of the close. 
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
#include "tundra/utils/ConsoleIO.h"
static void write_helper(Tundra_File *file, i64 write_result)
{
    Tundra_eprintf("Write helper with: %ld\n", write_result);

    // If result is an error.
    if(write_result < 0) return;

    // file->cursor_pos += write_result;

    if(file->do_writes_append)
    {
        file->cursor_pos = file->file_byte_size + write_result;
        Tundra_eprintf("Writes are appending. cursor position @ %ld\n",
            file->cursor_pos);
    }
    else 
    {
        file->cursor_pos += write_result;
        Tundra_eprintf("Writes are not appending. cursor position: %ld\n",
            file->cursor_pos);
    }
            
    // We've added bytes to the end of the file.
    if(file->cursor_pos > file->file_byte_size) 
        file->file_byte_size = file->cursor_pos;

    Tundra_eprintf("File size: %ld\n", file->file_byte_size);
}

bool Tundra_File_at_eof(const Tundra_File *file)
{
    return file->cursor_pos >= file->file_byte_size;
}

void Tundra_File_check_openerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to open file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_File_check_closeerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to close file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_File_check_writeerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to write to file: %s\n", 
        Tundra_err_to_rdbl(result));
}

void Tundra_File_check_readerr(i64 result)
{
    TUNDRA_RT_ASSERT(result >= 0, "Failed to read from file: %s\n", 
        Tundra_err_to_rdbl(result));
}

i64 Tundra_File_open(Tundra_File *file, const char *path, 
    Tundra_FileOpenMode open_mode, Tundra_FileWriteBehavior write_behavior,
    bool create_if_noexist, bool clear_file)
{
    if(file == NULL || path == NULL) return -TUNDRA_ERR_BADADDR;

    enum
    {
        TRUNCATE_FLAG = 512
    };

    i64 open_flags = open_mode | write_behavior |  
        (create_if_noexist * TUNDRA_LINUX_FILEOPENFLAG_CREATE) | 
        (clear_file * TRUNCATE_FLAG);

    i64 open_result = open_file_helper(path, open_flags, 0644);

    // If error
    if(open_result < 0) return open_result;

    InTundra_IBuff_init(&file->ibuff, open_result);
    InTundra_OBuff_init(&file->obuff, open_result);

    file->handle = open_result;

    i64 file_size = find_file_size(file);

    // If error
    if(file_size < 0) return file_size;

    file->file_byte_size = file_size;

    i64 cursor_pos = get_cursor_pos_in_file(file);

    // If error
    if(cursor_pos < 0) return cursor_pos;

    file->do_writes_append = 
        (write_behavior == TUNDRA_FILE_WRITE_BEHAVIOR_APPEND);

    file->cursor_pos = cursor_pos;

    file->last_op = INTUNDRA_FILE_OPERATION_NONE;

    return 0;
}

/**
 * @brief Clears the input buffer of a File and adjusts the File's 
 * 
 * @param file 
 * @return `i64` 
 */
static i64 reset_input_buff(Tundra_File *file)
{
    i64 result = InTundra_syscall(TUNDRA_LINUX_SYSCALL_LSEEK, file->handle, 
        -(i64)InTundra_IBuff_size(&file->ibuff), 
        TUNDRA_LINUX_SEEKBEHAVIOR_CUR, 0, 0, 0);

    if(result < 0) return result;

    InTundra_IBuff_clear(&file->ibuff);

    return result;
}

/**
 * @brief Checks if the user is writing to a file directly after reading and 
 * handles it if so.
 * 
 * If the user writes after reading, any cached read bytes in the input buffer 
 * must be cleared and the cursor inside the file handle must be moved back
 * to represent the user not "using" those cached read bytes. 
 * 
 * @param file File to check.
 * 
 * @return `i64` 0 if successful, otherwise it is the error from attempting to
 * move the cursor inside the file.
 */
static i64 check_write_after_read(Tundra_File *file)
{
    const u64 buff_size = InTundra_IBuff_size(&file->ibuff);

    // If we didn't read last or we don't have any leftover cached bytes, 
    // do nothing.
    if(file->last_op != INTUNDRA_FILE_OPERATION_READ || 
        buff_size == 0) return 0;

    i64 result = reset_input_buff(file);

    return (result < 0) ? result : 0;
}

i64 Tundra_File_write_cstr(Tundra_File *file, const char *cstr)
{
    if(file == NULL || cstr == NULL) return -TUNDRA_ERR_BADADDR;

    check_write_after_read(file);
    file->last_op = INTUNDRA_FILE_OPERATION_WRITE;

    u64 cstr_len = Tundra_get_cstr_len(cstr);

    i64 result = InTundra_OBuff_write_bytes(&file->obuff, (const u8*)cstr,
        cstr_len);

    // const i64 result = InTundra_raw_write_bytes(file->handle, cstr, 
    //     (i64)cstr_len);

    write_helper(file, result);
    return result;
}

#define WRITE_NUM_IMPL(type) \
i64 Tundra_File_write_##type(Tundra_File *file, type num) {\
    if(file == NULL) return -TUNDRA_ERR_BADADDR; \
    i64 result = check_write_after_read(file); \
    if(result < 0) return result; \
    file->last_op = INTUNDRA_FILE_OPERATION_WRITE; \
    result = InTundra_OBuff_write_##type(&file->obuff, num);\
    write_helper(file, result);\
    return result; }

WRITE_NUM_IMPL(char)
WRITE_NUM_IMPL(u64)
WRITE_NUM_IMPL(i64)
WRITE_NUM_IMPL(u32)
WRITE_NUM_IMPL(int)
WRITE_NUM_IMPL(u16)
WRITE_NUM_IMPL(i16)
WRITE_NUM_IMPL(u8)
WRITE_NUM_IMPL(i8)
WRITE_NUM_IMPL(float)

i64 Tundra_File_writef(Tundra_File *file, const char *format, ...)
{
    if(file == NULL || format == NULL) return -TUNDRA_ERR_BADADDR;

    Tundra_VaList args;
    Tundra_varg_start(args, format);

    i64 result = Tundra_File_vargs_writef(file, format, args);

    Tundra_varg_end(args);

    return result;
}

i64 Tundra_File_vargs_writef(Tundra_File *file, const char *format, 
    Tundra_VaList args)
{
    // InTundra_vargs_raw_write_fmt checks if format == NULL
    if(file == NULL ) return -TUNDRA_ERR_BADADDR;

    Tundra_DynamicArrayChar chars = 
        InTundra_vargs_convert_fmt_to_chars(format, args);

    check_write_after_read(file);
    file->last_op = INTUNDRA_FILE_OPERATION_WRITE;

    i64 result = InTundra_OBuff_write_bytes(&file->obuff, 
        (const u8*)Tundra_DynArrChar_data(&chars), 
        Tundra_DynArrChar_size(&chars));
        
    Tundra_DynArrChar_free(&chars);

    write_helper(file, result);

    return result;
}

static i64 reset_output_buff(Tundra_File *file)
{
    return InTundra_OBuff_flush(&file->obuff);
}

static i64 check_read_after_write(Tundra_File *file)
{
    if(file->last_op != INTUNDRA_FILE_OPERATION_WRITE) return 0;

    return reset_output_buff(file);
}

static void read_helper(Tundra_File *file, i64 read_result)
{
    // Tundra_eprintf("\nRead helper with %ld\n", read_result);
    // If result is an error.
    if(read_result < 0) return;

    file->cursor_pos += read_result;

    // Tundra_eprintf("Cursor position is now %ld\n", file->cursor_pos);
}

void Tundra_File_readin_bytes(Tundra_File *file, void *buffer, u64 num_bytes,
    i64 *read_result_output)
{
    if(file == NULL || buffer == NULL)
    {
        if(read_result_output != NULL) *read_result_output = TUNDRA_ERR_BADADDR;
        return;
    }

    if(num_bytes == 0) 
    {
        if(read_result_output != NULL) *read_result_output = TUNDRA_ERR_INVARG;
    }

    check_read_after_write(file);
    file->last_op = INTUNDRA_FILE_OPERATION_READ;
    
    i64 IBuff_result;
    InTundra_IBuff_readin_bytes(&file->ibuff, buffer, num_bytes, 
        &IBuff_result);
    read_helper(file, IBuff_result);
    if(read_result_output != NULL) *read_result_output = IBuff_result;
}

#define READ_NUM_IMPL(type) \
type Tundra_File_readin_##type(Tundra_File *file, i64 *read_result_output) { \
    check_read_after_write(file); \
    file->last_op = INTUNDRA_FILE_OPERATION_READ; \
    i64 result = 0; \
    type read = InTundra_IBuff_readin_##type(&file->ibuff, &result); \
    read_helper(file, result); \
    if(read_result_output != NULL) *read_result_output = result; \
    return read;}

READ_NUM_IMPL(char)

i64 Tundra_File_flush(Tundra_File *file)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    return InTundra_OBuff_flush(&file->obuff);
}

u64 Tundra_File_get_cursor(const Tundra_File *file)
{
    return (u64)file->cursor_pos;
}

i64 Tundra_File_set_cursor(Tundra_File *file, u64 position)
{
    // Since we are setting the cursor position, we don't need to worry about
    // any cached input bytes causing a desync of file offset with user offset.
    InTundra_IBuff_clear(&file->ibuff);

    i64 result = InTundra_OBuff_flush(&file->obuff);
    if(result < 0) return result;

    result = InTundra_syscall(
        TUNDRA_LINUX_SYSCALL_LSEEK,
        file->handle,
        (i64)position,
        TUNDRA_LINUX_SEEKBEHAVIOR_SET,
        0, 0, 0
    );
    if(result < 0) return result;

    file->cursor_pos = result;

    return result;
}

i64 Tundra_File_move_cursor(Tundra_File *file, i64 offset)
{
    // Unlike setting the cursor (above), we do need to worry about cached 
    // input bytes that would mistakenly put our file cursor ahead of where the
    // user thinks it is.
    i64 result = reset_input_buff(file);
    if(result < 0) return result;

    result = reset_output_buff(file);
    if(result < 0) return result;
    
    result = InTundra_syscall(
        TUNDRA_LINUX_SYSCALL_LSEEK,
        file->handle,
        offset,
        TUNDRA_LINUX_SEEKBEHAVIOR_CUR,
        0, 0, 0
    );
    if(result < 0) return result;

    file->cursor_pos = result;

    return result;
}

i64 Tundra_File_close(Tundra_File *file)
{
    if(file == NULL) return -TUNDRA_ERR_BADADDR;

    InTundra_OBuff_flush(&file->obuff);

    i64 close_result = close_file_helper(file->handle);

    if(close_result < 0) return close_result;

    file->handle = TUNDRA_IOHANDLE_INVALID;
    file->file_byte_size = 0;
    file->cursor_pos = 0;

    InTundra_IBuff_free(&file->ibuff);
    InTundra_OBuff_free(&file->obuff);

    return 0;
}

u64 Tundra_File_get_size(const Tundra_File *file)
{
    if(file == NULL) return 0;

    return (u64)file->file_byte_size;
}
