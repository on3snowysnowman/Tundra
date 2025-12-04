/**
 * @file FatalHandler.hpp
 * @author Joel Height (On3SnowySnwoman@gmail.com)
 * @brief Handler for when fatal crashes are reached, outputs a 
 * message and runs the fatal handling function.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

[[noreturn]] inline void TundraIn_FtlHnd_hard_trap()
{
    __builtin_trap();
    // for(;;) {}
}

// Accept va_list so we can forward varargs correctly
typedef void(*Tundra_fat_hand_t)(const char* file, int line, \
    const char* func, const char* fmt, va_list args);

static inline void Tundra_FtlHnd_default_fatal_handler(const char* file, 
    int line, const char* func, const char* fmt, va_list args)
{
    fprintf(stderr, "[%s:%d %s] -> ", file, line, func);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
}

static Tundra_fat_hand_t Tundra_FtlHnd_fatal_handler_func = 
    &Tundra_FtlHnd_default_fatal_handler;

static inline void Tundra_FtlHnd_set_fatal_handler(Tundra_fat_hand_t handler)
{
    Tundra_FtlHnd_fatal_handler_func = handler ? handler : 
        &Tundra_FtlHnd_default_fatal_handler;
}

[[noreturn]] static inline void Tundra_FtlHnd_fatal(const char* file, int line,
    const char* func, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Tundra_FtlHnd_fatal_handler_func(file, line, func, fmt, args);
    va_end(args);
    TundraIn_FtlHnd_hard_trap();
}

#define TUNDRA_FATAL(fmt, ...) \
    Tundra_FtlHnd_fatal(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
