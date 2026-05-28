/**
 * @file IOInterface.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for interacting with the OS for low level intput and output, 
 * including file handling.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_IOINTERFACE_H
#define TUNDRA_IOINTERFACE_H

#include "tundra/common/SystemInfo.h"
#include "tundra/common/TypeDef.h"
#include "tundra/common/VariadicArgs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef i64 InTundra_IOHandle;

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

// Handle definitions
#define TUNDRA_IOHANDLE_INVALID -1
#define TUNDRA_IOHANDLE_STDIN 0
#define TUNDRA_IOHANDLE_STDOUT 1
#define TUNDRA_IOHANDLE_ERROUT 2

#define TUNDRA_LINUX_WORKING_DIRECTORY_FD -100

#define TUNDRA_LINUX_FILEOPENFLAG_RDONLY 0 // read only
#define TUNDRA_LINUX_FILEOPENFLAG_WRONLY 1 // write only
#define TUNDRA_LINUX_FILEOPENFLAG_RDWR 2 // read and write
#define TUNDRA_LINUX_FILEOPENFLAG_CREATE 64 // create file if it does not exist

// Only meaningful when used with CREATE, fails if the file alreadyexists.
#define TUNDRA_LINUX_FILEOPENFLAG_EXCL 128 

// If the file already exists, erase contents.
#define TUNDRA_LINUX_FILEOPENFLAG_TRUNC 512 

// Append to the file's contents if it exists.
#define TUNDRA_LINUX_FILEOPENFLAG_APPEND 1024 


/**
 * @brief Writes a set number of bytes to the specified handle.  
 * If the return is negative, it's an error code. Otherwise it is
 * the number of bytes successfully written.
 * 
 * @param handle Handle to write to.
 * @param bytes Pointer to the bytes to write.
 * @param num_bytes Number of bytes to write.
 * 
 * @return i64 Number of bytes written if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 InTundra_write_bytes(InTundra_IOHandle handle, const void *bytes, 
    i64 num_bytes);

i64 InTundra_write_u64(InTundra_IOHandle handle, u64 num);

i64 InTundra_write_i64(InTundra_IOHandle handle, i64 num);

i64 InTundra_write_u32(InTundra_IOHandle handle, u32 num);

i64 InTundra_write_i32(InTundra_IOHandle handle, i32 num);

i64 InTundra_write_u16(InTundra_IOHandle handle, u16 num);

i64 InTundra_write_i16(InTundra_IOHandle handle, i16 num);

i64 InTundra_write_u8(InTundra_IOHandle handle, u8 num);

i64 InTundra_write_i8(InTundra_IOHandle handle, i8 num);

i64 InTundra_write_float(InTundra_IOHandle handle, float num);

i64 InTundra_write_formatted(InTundra_IOHandle handle, const char *format, ...);

i64 InTundra_vargs_write_formatted(InTundra_IOHandle handle, const char *format,
    Tundra_VaList args);

/**
 * @brief Reads a set number of bytes from the specified handle. 
 * If the return is negative, it's an error code. Otherwise it is
 * the number of bytes successfully read.
 * 
 * @param handle Handle to read from.
 * @param output Output buffer to place read bytes into.
 * @param num_bytes Number of bytes to read.
 * 
 * @return i64 Number of bytes read if the return is non negative, otherwise it 
 * is an error code. 
 */
i64 InTundra_read_bytes(InTundra_IOHandle handle, void *output, i64 num_bytes);

// /**
//  * @brief Returns the cursor position inside an open file. 
//  * If the return is negative it is an error code. Otherwise it is the byte 
//  * position of the cursor in the file.
//  * 
//  * @param handle File handle to inspect.
//  * 
//  * @return i64 Byte position of the cursor in the file if return is non 
//  * negative, otherwise it is an error code.
//  */
// i64 InTundra_get_cursor_pos_in_file(InTundra_IOHandle handle);

// /**
//  * @brief Moves the cursor inside an open file to a specified position.
//  * If the return is negative it is an error code. Otherwise it is the byte 
//  * position of the cursor in the file.
//  * 
//  * @param byte_position 
//  * 
//  * @return i64 
//  */
// i64 InTundra_move_cursor_in_file(InTundra_IOHandle, i64 byte_position);

// /**
//  * @brief Returns the file size of the file with the specified handle. 
//  * If the returns is negative, it's an error code. If it is non negative it is
//  * the size in bytes of the file.
//  * 
//  * @param handle File handle to inspect.
//  * 
//  * @return i64 Number of bytes in the file if return is non negative, otherwise
//  * it is an error code. 
//  */
// i64 InTundra_get_file_size(InTundra_IOHandle handle);

// /**
//  * @brief Attempts to open a file and returns the handle id.
//  * If the return is negative, it's an error code. Otherwise it is
//  * the the handle id.
//  * 
//  * The `path` to the file is a relative path from the current directory.
//  * 
//  * @param path Path to the file, relative to current directory.
//  * @param open_flags Flags for open behavior. Combination of Linux behavioral
//  * flags.
//  * @param create_privileges Only relevant when `open_flags` contains the CREATE
//  * flag, sets the user privileges of a newly created file.
//  * 
//  * @return InTundra_IOHandle File handle id if the return is non negative, 
//  * otherwise it is an error code. 
//  */
// InTundra_IOHandle InTundra_open_file(const char *path, 
//     i64 open_flags, i64 create_privileges);

// InTundra_IOHandle InTundra_close_file(InTundra_IOHandle file_handle);

#else // TUNDRA_SYS_x86_64
#error Not Implemented
#endif // TUNDRA_SYS_x86_64

#else // TUNDRA_PLATFORM_LINUX
#error Not Implemented
#endif // TUNDRA_PLATFORM_LINUX

#ifdef __cplusplus
}   
#endif


#endif