/**
 * @file CoreTypes.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for core types including fixed size integers.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once


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
typedef uint64 uintptr_t;

template<typename T>
struct always_false 
{ 
    static constexpr bool value = false; 
};

template<typename T>
struct is_signed_integer 
{
    constexpr static bool value = false;
};

template<>
struct is_signed_integer<Tundra::int8>
{
    constexpr static bool value = true;
};

template<>
struct is_signed_integer<Tundra::int16>
{
    constexpr static bool value = true;
};

template<>
struct is_signed_integer<Tundra::int32>
{
    constexpr static bool value = true;
};

template<>
struct is_signed_integer<Tundra::int64>
{
    constexpr static bool value = true;
};

} // namespace Tundra

#ifndef NULL
#define NULL ((void*)0)
#endif

static_assert(sizeof(Tundra::int8)  == 1, "int8 must be 1 byte");
static_assert(sizeof(Tundra::int16) == 2, "int16 must be 2 bytes");
static_assert(sizeof(Tundra::int32) == 4, "int32 must be 4 bytes");
static_assert(sizeof(Tundra::int64) == 8, "int64 must be 8 bytes");
static_assert(sizeof(void*) == 8, "Only 64-bit platforms supported");
