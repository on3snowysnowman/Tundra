/**
 * @file CoreTypes.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for core types including fixed size integer and NULL.
 * @version 0.1
 * @date 07-30-25
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


namespace Tundra
{

typedef signed char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned long long uint64;

typedef int64  intptr_t;
typedef Tundra::uint64 uintptr_t;

template<typename T>
struct always_false { static constexpr bool value = false; };

} // namespace Tundra

#ifndef NULL
#define NULL ((void*)0)
#endif

static_assert(sizeof(Tundra::int8)  == 1, "int8 must be 1 byte");
static_assert(sizeof(Tundra::int16) == 2, "int16 must be 2 bytes");
static_assert(sizeof(Tundra::int32) == 4, "int32 must be 4 bytes");
static_assert(sizeof(Tundra::int64) == 8, "int64 must be 8 bytes");
static_assert(sizeof(void*) == 8, "Only 64-bit platforms supported");
