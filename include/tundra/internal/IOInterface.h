/**
 * @file IOInterface.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for interacting with the OS for intput and output.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_IOINTERFACE_H
#define TUNDRA_IOINTERFACE_H

#include "tundra/utils/SystemInfo.h"
#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef i32 TundraIOHandle;

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

// Handle definitions
#define TUNDRA_STD_IN 0
#define TUNDRA_STD_OUT 1
#define TUNDRA_ERR_OUT 2

// Common Linux syscall errors
#define TUNDRA_EPERM   1   // Operation not permitted
#define TUNDRA_ENOENT  2   // No such file or directory
#define TUNDRA_EINTR   4   // Interrupted syscall
#define TUNDRA_EIO     5   // I/O error
#define TUNDRA_EBADF   9   // Bad file descriptor
#define TUNDRA_EAGAIN  11  // Try again / would block
#define TUNDRA_ENOMEM  12  // Out of memory
#define TUNDRA_EACCES  13  // Permission denied
#define TUNDRA_EFAULT  14  // Bad address
#define TUNDRA_EINVAL  22  // Invalid argument
#define TUNDRA_EPIPE   32  // Broken pipe

/**
 * @brief Writes a set number of bytes to the specified handle using syscalls. 
 * 
 * @param handle Handle to write to.
 * @param bytes Pointer to the bytes to write.
 * @param num_bytes Number of bytes to write.
 * 
 * @return i64 Error result of the syscall. 0 if successful. 
 */
i64 InTundra_write_bytes(TundraIOHandle handle, const void *bytes, 
    u64 num_bytes);

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