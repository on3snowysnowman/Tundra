/**
 * @file DynamicArrayTemplate.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Template for a DynamicArray component with specified type. Used by a 
 * generator script to create type-specific DynamicArrays.
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef NAME
#define NAME Tundra_DynamicArrayint
#endif

#ifndef TYPE
#define TYPE int
#endif  

#ifndef FUNC_NAME
#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _##name)
#endif  

#ifndef INT_FUNC_NAME
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, TYPE, \
    _##name)
#endif

#include "DynamicArrayTemplate.h"

#include <stdlib.h>
#include <string.h>


// Definitions -----------------------------------------------------------------

// Default capacity in elements of an Array.
#define DEF_CAP 4


// Internal Methods ------------------------------------------------------------

void INT_FUNC_NAME(init)(NAME *arr, uint64 init_cap)
{
    arr->data = (TYPE*)malloc(init_cap * sizeof(TYPE));
    
    arr->num_elem = 0;
    arr->cap = init_cap;
    arr->copy_func = INT_FUNC_NAME(def_copy);
}

void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem)
{
    memcpy((void*)dst, (const void*)src, num_elem * sizeof(TYPE));
}


// Public Methods --------------------------------------------------------------

void FUNC_NAME(init)(NAME *arr)
{
    INT_FUNC_NAME(init)(arr, DEF_CAP);
}

void FUNC_NAME(init_w_cap)(NAME *arr, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? DEF_CAP : init_cap; 

    INT_FUNC_NAME(init)(arr, init_cap);
}

void FUNC_NAME(init_w_elems)(NAME *arr, const TYPE *elements, 
    uint64 num_elem, bool strict_alloc)
{
}