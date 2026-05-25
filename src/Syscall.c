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


#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

i64 InTundra_syscall(i64 number, i64 arg0, i64 arg1, i64 arg2)
{
    i64 result;

    __asm__ volatile 
    (
        "syscall"
        : "=a"(result)
        : "a"(number),
        "D"(arg0),
        "S"(arg1),
        "d"(arg2)
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