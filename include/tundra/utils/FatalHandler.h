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

#include "tundra/common/VariadicArgs.h"

#ifdef __cplusplus
extern "C" {
#endif

[[noreturn]] void InTundra_FtlHnd_hard_trap(void);


// Accept Tundra_VaList so we can forward varargs correctly
typedef void(*Tundra_fat_hand_t)(const char* file, int line, \
    const char* func, const char* fmt, Tundra_VaList args);

[[noreturn]] void Tundra_FtlHnd_default_fatal_handler(
    const char* file, int line, const char* func, const char* fmt, 
    Tundra_VaList args);

void Tundra_FtlHnd_set_fatal_handler(Tundra_fat_hand_t handler);

void Tundra_FtlHnd_fatal(const char* file, int line,
    const char* func, const char* fmt, ...);

#define TUNDRA_FATAL(fmt, ...) \
    Tundra_FtlHnd_fatal(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
} // extern "C"
#endif

#endif // TUNDRA_FATALHANDLER_H
