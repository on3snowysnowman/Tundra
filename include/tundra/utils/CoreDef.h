/**
 * @file CoreDef.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for core types including fixed size integers and NULL,
 * as well as defining compile time assertion.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_CORETYPE_H
#define TUNDRA_CORETYPE_H

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/FatalHandler.h"

#define TUNDRA_CT_ASSERT(cond) typedef char \
    TUNDRA_CONCAT(TUNDRA_CT_ASSERTION, __COUNTER__)[(cond) ? 1 : -1]

#define TUNDRA_RT_ASSERT(expr, msg) \
    ((expr) ? (void)(0) : TUNDRA_FATAL(msg))

#define TUNDRA_ALIGN(alignment) \
    __attribute__((aligned(alignment)))

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

#ifndef NULL
#define NULL 0
#endif

TUNDRA_CT_ASSERT(sizeof(i8) == 1);
TUNDRA_CT_ASSERT(sizeof(i16) == 2);
TUNDRA_CT_ASSERT(sizeof(i32) == 4);
TUNDRA_CT_ASSERT(sizeof(i64) == 8);
TUNDRA_CT_ASSERT(sizeof(void*) == 8);

#endif
