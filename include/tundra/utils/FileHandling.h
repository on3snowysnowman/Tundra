/**
 * @file FileHandling.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing files. Opening and closing as well as writing 
 * and reading.
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_FILEHANDLER_H
#define TUNDRA_FILEHANDLER_H

#include "tundra/internal/IOInterface.h"
#include "tundra/internal/IOBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUNDRA_FILE_OPEN_MODE_READONLY 0
#define TUNDRA_FILE_OPEN_MODE_WRITEONLY 1
#define TUNDRA_FILE_OPEN_MODE_READWRITE 2

// #define TUNDRA_FILE_WRITE_BEHAVIOR_TRUNCATE 512

// No write behavior, the File is opened in read only mode.
#define TUNDRA_FILE_WRITE_BEHAVIOR_NONE 0
// Writes to a file are placed at the file cursor.
#define TUNDRA_FILE_WRITE_BEHAVIOR_AT_CURSOR 0
// Writes to a file are always placed at the end of the file.
#define TUNDRA_FILE_WRITE_BEHAVIOR_APPEND 1024

typedef i64 Tundra_FileOpenMode;

// typedef i64 Tundra_FileOpenBehavior;
typedef i64 Tundra_FileWriteBehavior;

typedef enum
{
    INTUNDRA_FILE_OPERATION_NONE,
    INTUNDRA_FILE_OPERATION_READ,
    INTUNDRA_FILE_OPERATION_WRITE
    
} InTundra_File_Operation;

typedef struct
{

    // If writes to this File are in append mode.
    bool do_writes_append;
    InTundra_IOHandle handle;
    InTundra_InputBuffer ibuff;
    InTundra_OutputBuffer obuff;
    InTundra_File_Operation last_op;
    i64 file_byte_size;
    i64 cursor_pos;
} Tundra_File;

/**
 * @brief Returns true if the open File's cursor is at the end of the File.
 * 
 * @param file File to query.
 * 
 * @return `bool` True if at end of file. 
 */
bool Tundra_File_at_eof(const Tundra_File *file);

/**
 * @brief Checks the return result of a file open call. If the result is an
 * error, fatals and outputs a the message. Otherwise, does nothing.
 * 
 * @param result Open result to check.
 */
void Tundra_File_check_openerr(i64 result);

/**
 * @brief Checks the return result of a file close call. If the result is an
 * error, fatals and outputs a the message. Otherwise, does nothing.
 * 
 * @param result Open result to check.
 */
void Tundra_File_check_closeerr(i64 result);

/**
 * @brief Checks the return result of a file write call. If the result is an
 * error, fatals and outputs a the message. Otherwise, does nothing.
 * 
 * @param result Open result to check.
 */
void Tundra_File_check_writeerr(i64 result);

/**
 * @brief Checks the return result of a file read call. If the result is an
 * error, fatals and outputs a the message. Otherwise, does nothing.
 * 
 * @param result Open result to check.
 */
void Tundra_File_check_readerr(i64 result);

// /**
//  * @brief Opens a file, initializing the passed `file` with the opened file's 
//  * data. If the return is negative, it's an error code. Otherwise, open was 
//  * successful.
//  * 
//  * `file` should be an uninitialized Tundra_File object, all contents will be 
//  * overwritten. 
//  * 
//  * @param file Uninitialized Tundra_File to populate with the opened file data.
//  * @param path Relative to current directory path to the file to open.
//  * @param open_mode Open mode for opening, ie Readonly, Writeonly.
//  * @param open_behavior Open behavior for opening, ie Appending / Truncating.
//  * Only relevant if writing.
//  * @param create_if_noexist Create the file if it does not exist.
//  * 
//  * @return `i64` Success if non negative, otherwise error code. 
//  */
i64 Tundra_File_open(Tundra_File *file, const char *path, 
    Tundra_FileOpenMode open_mode, Tundra_FileWriteBehavior write_behavior, 
    bool create_if_noexist, bool clear_file);

/**
 * @brief Writes a C String buffer to an open file. If the return is negative,
 * it's an error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param cstr C String to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_cstr(Tundra_File *file, const char *cstr);

/**
 * @brief Writes a char to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param c Char to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_char(Tundra_File *file, char c);


/**
 * @brief Writes an u64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num u64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_u64(Tundra_File *file, u64 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_i64(Tundra_File *file, i64 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_u32(Tundra_File *file, u32 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_int(Tundra_File *file, i32 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_u16(Tundra_File *file, u16 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_i16(Tundra_File *file, i16 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_u8(Tundra_File *file, u8 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_i8(Tundra_File *file, i8 num);


/**
 * @brief Writes an i64 to an open file. If the return is negative, it's an 
 * error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param file File to write to.
 * @param num i64 to write.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_write_float(Tundra_File *file, float num);

/**
 * @brief Writes a formatted C String to an open file. If the return is 
 * negative, it's an error code. Otherwise it is the number of bytes 
 * successfully written.
 * 
 * @param file File to write to.
 * @param format Formatted C String.
 * @param ... Arguments for the formatted C String.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_writef(Tundra_File *file, const char *format, ...);

/**
 * @brief Writes a formatted C String with explicit VArgs to an open file. If 
 * the return is negative, it's an error code. Otherwise it is the number of 
 * bytes successfully written.
 * 
 * @param file File to write to.
 * @param format Formatted C String.
 * @param args Arguments for the formatted C String.
 * 
 * @return `i64` Number of bytes written if non negative, otherwise it is an 
 * error code. 
 */
i64 Tundra_File_vargs_writef(Tundra_File *file, const char *format,
    Tundra_VaList args);

/**
 * @brief Reads a specified number of bytes from an open file into a buffer.
 * 
 * The buffer needs to be large enough to hold at least `num_bytes` bytes.
 * 
 * @param file File to read from.
 * @param buffer Buffer to place read bytes in.
 * @param num_bytes Number of bytes to read.
 * 
 * @return `i64` Number of bytes read if non negative, otherwise it is an error
 * code. 
 */
void Tundra_File_readin_bytes(Tundra_File *file, void *buffer, u64 num_bytes,
    i64 *read_result_output);

char Tundra_File_readin_char(Tundra_File *file, i64 *read_result_output);

/**
 * @brief Flushes buffered text to the open file's handle.
 * 
 * @param file File to flush.
 * 
 * @return `i64`  
 */
i64 Tundra_File_flush(Tundra_File *file);

/**
 * @brief Returns the cursor position in the open File.
 * 
 * @param file 
 * @return `u64` 
 */
u64 Tundra_File_get_cursor(const Tundra_File *file);

/**
 * @brief Attempts to set the cursor in an open File to a byte position relative
 * to the beginning of the File. Returns the result.
 * 
 * @param file 
 * @return `i64` 
 */
i64 Tundra_File_set_cursor(Tundra_File *file, u64 position);

/**
 * @brief Attempts to move the cursor in an open File by an amount relative to
 * its current position. Returns the result.
 * 
 * @param file 
 * @param offset 
 * @return `i64` 
 */
i64 Tundra_File_move_cursor(Tundra_File *file, i64 offset);

/**
 * @brief Closes an open file. If the return is negative, it's an error code. 
 * Otherwise, close was successful.
 * 
 * Leaves `file` in an uninitialized state.
 * 
 * @param file File to close.
 * 
 * @return `i64` Success if non negative, otherwise error code. 
 */
i64 Tundra_File_close(Tundra_File *file);

/**
 * @brief Returns the file size in bytes. Does not need to do any inspecting or
 * parsing of the file, this function is just a variable return.
 * 
 * @param file File to return size of. 
 * 
 * @return `u64` File size in bytes.
 */
u64 Tundra_File_get_size(const Tundra_File *file);

#ifdef __cplusplus
}
#endif

#endif