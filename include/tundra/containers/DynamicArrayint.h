#ifndef TUNDRA_DYNARRINT_H
#define TUNDRA_DYNARRINT_H

/* Copyright (C) 1991-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Template for a DynamicArray component with specified type. Used by a 
 * generator script to create type-specific DynamicArrays.
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/
/**
 * @file MacroHelper.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple Macros for helping with Macro expansion and creation.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
*/
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
/**
 * @file MacroHelper.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple Macros for helping with Macro expansion and creation.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
*/
/* Copyright (C) 1998-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */
/*
 * ISO C Standard:  7.16  Boolean type and values  <stdbool.h>
 */
/* bool, true and false are keywords.  */
/* Signal that all the definitions are present.  */
typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef int64 intptr_t;
typedef uint64 uintptr_t;
typedef char TUNDRA_ASSERT46[(sizeof(int8) == 1) ? 1 : -1];
typedef char TUNDRA_ASSERT47[(sizeof(int16) == 2) ? 1 : -1];
typedef char TUNDRA_ASSERT48[(sizeof(int32) == 4) ? 1 : -1];
typedef char TUNDRA_ASSERT49[(sizeof(int64) == 8) ? 1 : -1];
typedef char TUNDRA_ASSERT50[(sizeof(void*) == 8) ? 1 : -1];
// Containers ------------------------------------------------------------------
/**
 * @brief Automatic resizing contiguous container for storing procedurally 
 * added elements.
 *
 * Must be initialized using either the `init`, `copy` or `move` methods before 
 * use. Must be freed on end of use using the `free` method.
 *
 * Internals are read-only.
 */
typedef struct Tundra_DynamicArrayint
{
    // Heap allocated array of elements.
    int *data;
    // Number of elements currently added to the Array.
    uint64 num_elem;
    // Current maximum number of elements that can be added to the Array before 
    // it needs to be resized.
    uint64 cap;
    // Copy function invoked when the `copy` method is called. 
    void (*copy_func)(const int*, int*, uint64);
    // Free function invoked when the `free` method is called.
    // void (*free_func)(TYPE*, uint64);
} Tundra_DynamicArrayint;
// Internal Methods ------------------------------------------------------------
void TundraIn_DynArrint_init(Tundra_DynamicArrayint *arr, uint64 init_cap);
void TundraIn_DynArrint_def_copy(const int *src, int *dst,
    uint64 num_elem);
// Public Methods --------------------------------------------------------------
/**
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * sets internal components.
 *
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 * 
 * @param arr Array to init. 
*/
void Tundra_DynArrint_init(Tundra_DynamicArrayint *arr);
/**
 * @brief Initialize an Array with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Array is initialized with default capacity.
 *
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 * 
 * @param arr Array to init, 
 * @param init_cap Specified initial capacity.
 */
void Tundra_DynArrint_init_w_cap(Tundra_DynamicArrayint *arr, uint64 init_cap);
/**
 * @brief Initializes an Array with initial elements. Allocates memory and sets
 * internal components.
 *
 * `elements` are copied into the Array. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated for the Array. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 *
 * `elements` must not be memory inside the Array.
 * 
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
void Tundra_DynArrint_init_w_elems(Tundra_DynamicArrayint *arr, const int *elements,
    uint64 num_elem, _Bool strict_alloc);

#endif /* TUNDRA_DYNARRINT_H */
