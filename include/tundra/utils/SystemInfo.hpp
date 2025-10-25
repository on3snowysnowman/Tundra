/**
 * @file SystemInfo.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Tracks information about the system executing the program.
 * @version 0.1
 * @date 10-10-25
 *
 * @copyright Copyright (c) 2025
*/

#pragma once


// OS Check
#ifdef _WIN32
#define TUNDRA_PLATFORM_WINDOWS
#endif

#if defined(__linux__) 
#define TUNDRA_PLATFORM_LINUX

#elif defined(__APPLE__)
#define TUNDRA_PLATFORM_APPLE

#endif

#if defined(TUNDRA_PLATFORM_LINUX) || defined(TUNDRA_PLATFORM_APPLE)
#define TUNDRA_PLATFORM_POSIX
#endif

#if !defined(TUNDRA_PLATFORM_WINDOWS) && !defined(TUNDRA_PLATFORM_POSIX)
#error Unsupported OS
#endif

// Architecture check
#ifdef __x86_64__
#define TUNDRA_SYS_x86_64
#endif

#ifdef __aarch64__
#define TUNDRA_SYS_ARM
#endif

#if !defined(TUNDRA_SYS_x86_64) && !defined(TUNDRA_SYS_ARM)
#error Unsupported Architecture
#endif

#if !defined(__GNUC__) && !defined(__clang__)
#error Unsupported Compiler
#endif

// #include "tundra/utils/CoreTypes.hpp"


// namespace Tundra
// {

// enum Sys_OS
// {   
//     LINUX,
//     WINDOWS,
//     APPLE
// };

// enum Sys_Architecture
// {
//     ARM,
//     X86_64
// };

/**
 * @brief Generic info about the system executing the program.
 */
// struct SysInfo
// {

// static inline Tundra::uint8 mem_page_size;
// static inline Sys_OS op_sys;
// static inline Sys_Architecture architecture;

// };

// inline void init_sys_info();

// }; // namespace Tundra
