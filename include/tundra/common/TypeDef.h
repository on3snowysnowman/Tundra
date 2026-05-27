/**
 * @file TypeDef.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for core types including fixed size integers and NULL.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_CORETYPE_H
#define TUNDRA_CORETYPE_H

#ifndef TUNDRA_NO_CUSTOM_TYPES

#ifndef __cplusplus
#if defined __STDC_VERSION__ && __STDC_VERSION__ <= 201710L
#define bool _Bool
#define true 1
#define false 0
#endif
#else
// C++ is defined
#define _Bool bool
#endif

typedef signed char i8;
typedef unsigned char u8;

typedef short i16;
typedef unsigned short u16;

typedef int i32;
typedef unsigned int u32;

typedef long long i64;
typedef unsigned long long u64;

typedef unsigned long long uintptr;

#endif

/**
 * @brief Data for a CStr. Contains string pointer and length.
 */
typedef struct
{
    char *str;
    u64 length;
} Tundra_CStr;

#ifndef NULL
#define NULL 0
#endif

#endif
