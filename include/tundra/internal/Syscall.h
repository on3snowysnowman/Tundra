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

#include "tundra/utils/SystemInfo.h"
#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

#define TUNDRA_LINUX_SYSCALL_WRITE 1

i64 InTundra_syscall(i64 number, i64 arg0, i64 arg1, i64 arg2);

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