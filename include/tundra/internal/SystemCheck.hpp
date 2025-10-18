/**
 * @file SystemCheck.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Macro check for supported Platform, Compiler and OS.
 * @version 0.1
 * @date 10-10-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#ifdef _WIN32
#define TUNDRA_PLATFORM_WINDOWS
#endif

#if defined(__linux__) || defined(__APPLE__)
#define TUNDRA_PLATFORM_POSIX
#endif

#if !defined(TUNDRA_PLATFORM_WINDOWS) && !defined(TUNDRA_PLATFORM_POSIX)
#error Unsupported OS
#endif

#if !defined(__aarch64__) && !defined(__x86_64__)
#error Unsupported Architecture
#endif

// #if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
// #error Unsupported OS
// #endif

#if !defined(__GNUC__) && !defined(__clang__)
#error Unsupported Compiler
#endif