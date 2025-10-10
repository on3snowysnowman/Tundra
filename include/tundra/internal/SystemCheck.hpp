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

#if !defined(__aarch64__) && !defined(__x86_64__)
#error Unsupported Architecture
#endif

#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
#error Unsupported OS
#endif

#if !defined(__GNUC__) && !defined(__clang__)
#error Unsupported Compiler
#endif