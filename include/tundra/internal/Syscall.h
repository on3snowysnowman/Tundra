/**
 * @file Syscall.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for handling system calls across platforms.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_SYSCALL_H
#define TUNDRA_SYSCALL_H

#include "tundra/common/SystemInfo.h"
#include "tundra/common/TypeDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

#define TUNDRA_LINUX_SYSCALL_READ 0
#define TUNDRA_LINUX_SYSCALL_WRITE 1
#define TUNDRA_LINUX_SYSCALL_CLOSE 3
#define TUNDRA_LINUX_SYSCALL_LSEEK 8
#define TUNDRA_LINUX_SYSCALL_EXIT 60
#define TUNDRA_LINUX_SYSCALL_OPENAT 257

#define TUNDRA_LINUX_SEEKBEHAVIOR_SET 0 // from beginning
#define TUNDRA_LINUX_SEEKBEHAVIOR_CUR 1 // from current position
#define TUNDRA_LINUX_SEEKBEHAVIOR_END 2 // from end

// Common Linux syscall errors
#define TUNDRA_ERR_OPDENY 1   // Operation not permitted
#define TUNDRA_ERR_NOENT 2   // No such file or directory
#define TUNDRA_ERR_INTR 4   // Interrupted syscall
#define TUNDRA_ERR_IO 5   // I/O error
#define TUNDRA_ERR_BADF 9   // Bad file descriptor
#define TUNDRA_ERR_AGAIN 11  // Try again / would block
#define TUNDRA_ERR_NOMEM 12  // Out of memory
#define TUNDRA_ERR_PERMDENY 13  // Permission denied
#define TUNDRA_ERR_BADADDR 14  // Bad address
#define TUNDRA_ERR_INVARG 22  // Invalid argument

i64 InTundra_syscall(i64 number, i64 arg0, i64 arg1, i64 arg2, i64 arg3,
    i64 arg4, i64 arg5);

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