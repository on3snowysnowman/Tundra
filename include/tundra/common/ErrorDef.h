/**
 * @file ErrorDef.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Error definitions and converting them to a readable message.
 * @version 0.1
 * @date 2026-05-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_ERRORDEF_H
#define TUNDRA_ERRORDEF_H

#include "tundra/common/TypeDef.h"
#include "tundra/common/SystemInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

// Common Linux syscall errors
#define TUNDRA_EPERM 1   // Operation not permitted
#define TUNDRA_ENOENT 2   // No such file or directory
#define TUNDRA_EINTR 4   // Interrupted syscall
#define TUNDRA_EIO 5   // I/O error
#define TUNDRA_EBADF 9   // Bad file descriptor
#define TUNDRA_EAGAIN 11  // Try again / would block
#define TUNDRA_ENOMEM 12  // Out of memory
#define TUNDRA_EACCES 13  // Permission denied
#define TUNDRA_EFAULT 14  // Bad address
#define TUNDRA_EINVAL 22  // Invalid argument

const char* Tundra_err_to_rdbl(i64 error);

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
