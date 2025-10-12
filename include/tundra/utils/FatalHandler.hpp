/**
 * @file FatalHandler.hpp
 * @author Joel Height (On3SnowySnwoman@gmail.com)
 * @brief Handler for when fatal crashes are reached, outputs a 
 *    message and runs the fatal handling function.
 * @version 0.1
 * @date 2025-10-12
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include <cstdarg>
#include <cstdio>

namespace Tundra
{

namespace FtlHnd
{

namespace Internal
{

[[noreturn]] inline void hard_trap()
{
    __builtin_trap();
    // for(;;) {}
}

} // namespace Internal

// Accept va_list so we can forward varargs correctly
using fatal_handler_t = void(*)(const char* file, int line,
    const char* func, const char* fmt, va_list args);

inline void default_fatal_handler(const char* file, int line,
    const char* func, const char* fmt, va_list args)
{
    fprintf(stderr, "[%s:%d %s] -> ", file, line, func);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
}

fatal_handler_t fatal_handler_func = &default_fatal_handler;

inline void set_fatal_handler(fatal_handler_t handler)
{
    fatal_handler_func = handler ? handler : &default_fatal_handler;
}

[[noreturn]] inline void fatal(const char* file, int line,
    const char* func, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Tundra::FtlHnd::fatal_handler_func(file, line, func, fmt, args);
    va_end(args);
    Tundra::FtlHnd::Internal::hard_trap();
}

} // namespace FtlHnd
} // namespace Tundra

#define TUNDRA_FATAL(fmt, ...) \
    Tundra::FtlHnd::fatal(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

