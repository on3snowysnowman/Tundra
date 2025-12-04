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
#include <stdbool.h>

#define TUNDRA_ASSERT(expr) \
    typedef char TUNDRA_CONCAT(TUNDRA_ASSERT, __LINE__)[(expr) ? 1 : -1]

#ifndef TUNDRA_NO_CUSTOM_TYPES

#ifndef bool
#define bool _Bool
#endif

typedef signed char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned long long uint64;

// typedef int64  intptr_t;
// typedef uint64 uintptr_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

TUNDRA_ASSERT(sizeof(int8)  == 1);
TUNDRA_ASSERT(sizeof(int16) == 2);
TUNDRA_ASSERT(sizeof(int32) == 4);
TUNDRA_ASSERT(sizeof(int64) == 8);
TUNDRA_ASSERT(sizeof(void*) == 8);

#endif
#endif