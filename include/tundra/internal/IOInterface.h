/**
 * @file IOInterface.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for reading and writing to specified handles for primitive 
 * input and output.
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
#include "tundra/containers/DynamicArrayChar.h"


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
i64 InTundra_raw_write_bytes(InTundra_IOHandle handle, const void *bytes, 
    i64 num_bytes);

/**
 * @brief Writes a u64 to the specified handle. If the return is negative, it's 
 * an error code. Otherwise it is the number of bytes successfully written.
 * 
 * @param handle Handle to write to.
 * @param num u64 to write.
 * 
 * @return i64 Number of bytes written if the return is non negative, otherwise
 * it is an error code. 
 */
i64 InTundra_raw_write_u64(InTundra_IOHandle handle, u64 num);

i64 InTundra_raw_write_i64(InTundra_IOHandle handle, i64 num);

i64 InTundra_raw_write_u32(InTundra_IOHandle handle, u32 num);

i64 InTundra_raw_write_i32(InTundra_IOHandle handle, i32 num);

i64 InTundra_raw_write_u16(InTundra_IOHandle handle, u16 num);

i64 InTundra_raw_write_i16(InTundra_IOHandle handle, i16 num);

i64 InTundra_raw_write_u8(InTundra_IOHandle handle, u8 num);

i64 InTundra_raw_write_i8(InTundra_IOHandle handle, i8 num);

i64 InTundra_raw_write_float(InTundra_IOHandle handle, float num);

Tundra_DynamicArrayChar InTundra_convert_fmt_to_chars(const char *format,
    ...);

Tundra_DynamicArrayChar InTundra_convert_vargs_to_chars(const char *format,
    Tundra_VaList args);

i64 InTundra_raw_write_fmt(InTundra_IOHandle handle, const char *format, 
    ...);

i64 InTundra_vargs_raw_write_fmt(InTundra_IOHandle handle, const char *format,
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
i64 InTundra_raw_read_bytes(InTundra_IOHandle handle, void *output, 
    i64 num_bytes);

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
