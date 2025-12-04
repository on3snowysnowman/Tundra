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
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"

#include <stdlib.h>
#include <string.h>


// Definitions -----------------------------------------------------------------

// Default capacity in elements of an Array.
#define DEF_CAP 4


// Internal Methods ------------------------------------------------------------

void INT_FUNC_NAME(init)(NAME *arr, uint64 init_cap)
{
    arr->data = (TYPE*)Tundra_alloc_mem(init_cap * sizeof(TYPE));
    
    arr->num_elem = 0;
    arr->cap = init_cap;
    arr->copy_func = INT_FUNC_NAME(def_copy);
    arr->free_func = INT_FUNC_NAME(def_free);
}

void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem)
{
    memcpy((void*)dst, (const void*)src, num_elem * sizeof(TYPE));
}

void INT_FUNC_NAME(def_free)(TYPE *mem, uint64 /** num_elem */ )
{
    Tundra_free_mem((void*)mem);
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
    arr->copy_func = INT_FUNC_NAME(def_copy);
    arr->free_func = INT_FUNC_NAME(def_free);

    const uint64 NUM_CPY_BYTE = num_elem * sizeof(TYPE);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        arr->data = (TYPE*)(Tundra_alloc_mem(NUM_CPY_BYTE));
        arr->copy_func(elements, arr->data, num_elem);
        // copy_mem_fwd(elements, arr->data, NUM_CPY_BYTE);
        arr->num_elem = num_elem;
        arr->cap = num_elem;
        return;
    }

    // -- Use the "reserving" method to alloc, which will generally alloc more 
    // space than is needed to prevent immediate expansion on next add call. --

    // Temp var for retrieving the capacity of the allocated block through the
    // reserve call. Capacity in bytes, not elements.
    uint64 temp_cap_bytes;

    Tundra_alloc_reserve_mem(
        (void**)(&arr->data), 
        &temp_cap_bytes, 
        NUM_CPY_BYTE);

    // copy_mem_fwd(elements, arr->data, NUM_CPY_BYTE);
    arr->copy_func(elements, arr->data, num_elem);

    arr->num_elem = num_elem;
    arr->cap = temp_cap_bytes / sizeof(TYPE);
}

void FUNC_NAME(free)(NAME *arr)
{
    arr->free_func(arr->data, arr->num_elem);
    arr->data = NULL;
    arr->copy_func = NULL;
    arr->free_func = NULL;
}