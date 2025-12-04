/**
 * @file SystemInfo.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Tracks information about the system executing the program.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_SYSTEMINFO_H
#define TUNDRA_SYSTEMINFO_H

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
#endif
