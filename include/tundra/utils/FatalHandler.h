/**
 * @file FatalHandler.hpp
 * @author Joel Height (On3SnowySnwoman@gmail.com)
 * @brief Handler for when fatal crashes are reached, outputs a 
 * message and runs the fatal handling function.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_FATALHANDLER_H
#define TUNDRA_FATALHANDLER_H

#ifndef TUNDRA_NOLIBC
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

[[noreturn]] static inline void InTundra_FtlHnd_hard_trap(void)
{
    __builtin_trap();
    // for(;;) {}
}

#ifndef TUNDRA_NOLIBC
// Accept va_list so we can forward varargs correctly
typedef void(*Tundra_fat_hand_t)(const char* file, int line, \
    const char* func, const char* fmt, va_list args);

[[noreturn]] static inline void Tundra_FtlHnd_default_fatal_handler(const char* file, 
    int line, const char* func, const char* fmt, va_list args)
{
    fprintf(stderr, "[%s:%d %s] -> ", file, line, func);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    InTundra_FtlHnd_hard_trap();
}

static Tundra_fat_hand_t Tundra_FtlHnd_fatal_handler_func = 
    &Tundra_FtlHnd_default_fatal_handler;

static inline void Tundra_FtlHnd_set_fatal_handler(Tundra_fat_hand_t handler)
{
    Tundra_FtlHnd_fatal_handler_func = handler ? handler : 
        &Tundra_FtlHnd_default_fatal_handler;
}

static inline void Tundra_FtlHnd_fatal(const char* file, int line,
    const char* func, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Tundra_FtlHnd_fatal_handler_func(file, line, func, fmt, args);
    va_end(args);
}

#define TUNDRA_FATAL(fmt, ...) \
    Tundra_FtlHnd_fatal(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define TUNDRA_FATAL(fmt, ...) InTundra_FtlHnd_hard_trap()
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TUNDRA_FATALHANDLER_H
