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

/**
 * @brief Wrapper for a builtin hard trap.
 */
[[noreturn]] void InTundra_hard_trap(void);

// Accept Tundra_VaList so we can forward varargs correctly
typedef void(*Tundra_fat_hand_t)(const char* file, int line, \
    const char* func, const char* fmt, Tundra_VaList args);

/**
 * @brief The default fatal handling function. Simply outputs the error message
 * and hard traps.
 * 
 * @param file Name of the file where the fatal is called.
 * @param line Line where the fatal is called.
 * @param func Function where the fatal was called.
 * @param fmt Formatted message to output.
 * @param args Arguments for the formatted message.
 */
[[noreturn]] void Tundra_dflt_fatal_func(
    const char* file, int line, const char* func, const char* fmt, 
    Tundra_VaList args);

/**
 * @brief Sets the fatal handling function.
 * 
 * This function is what is called when a fatal is triggered using 
 * `TUNDRA_FATAL`.
 * 
 * @param handler 
 */
void Tundra_set_fatal_handler(Tundra_fat_hand_t handler);

/**
 * @brief Outputs a message and calls the fatal handling function. 
 * 
 * @param file Name of the file where the fatal is called.
 * @param line Line where the fatal is called.
 * @param func Function where the fatal was called.
 * @param fmt Formatted message to output.
 * @param ... Arguments for the formatted message.
 */
void Tundra_fatal_func(const char* file, int line,
    const char* func, const char* fmt, ...);

// Outputs a message and calls the fatal function with error location 
// information. Accepts variadic arguments for formatted message printing.
#define TUNDRA_FATAL(fmt, ...) \
    Tundra_fatal_func(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
} // extern "C"
#endif

#endif // TUNDRA_FATALHANDLER_H
