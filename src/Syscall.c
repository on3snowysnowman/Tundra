/**
 * @file Syscall.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for handling system calls across platforms.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/Syscall.h"
#include "tundra/utils/MemUtils.h"

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

i64 InTundra_syscall(i64 number, i64 arg0, i64 arg1, i64 arg2, i64 arg3, 
    i64 arg4, i64 arg5)
{
    i64 result;

    register i64 r10 __asm__("r10") = arg3;
    register i64 r8  __asm__("r8")  = arg4;
    register i64 r9  __asm__("r9")  = arg5;

    __asm__ volatile
    (
        "syscall"
        : "=a"(result)
        : "a"(number),
          "D"(arg0),
          "S"(arg1),
          "d"(arg2),
          "r"(r10),
          "r"(r8),
          "r"(r9)
        : "rcx", "r11", "memory"
    );

    return result;
}

#else // TUNDRA_SYS_x86_64
#error Not Implemented
#endif // TUNDRA_SYS_x86_64

#else // TUNDRA_PLATFORM_LINUX
#error Not Implemented
#endif // TUNDRA_PLATFORM_LINUX