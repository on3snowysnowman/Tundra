/**
 * @file DynamicStack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container providing LIFO behavior for storing
 * procedurally added elements.
 * @date 2025-12-06
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/utils/CoreDef.h"
#include "tundra/utils/NumLimits.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/Math.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_DYNSTK_H
#define TUNDRA_DYNSTK_H
#define TUNDRA_DYNSTK_DEF_CAP 4
#endif 

#ifndef TUNDRA_TYPE 
#define TUNDRA_TYPE int
#endif

#ifndef TUNDRA_TYPENAME
#define TUNDRA_TYPENAME int
#endif

#define TUNDRA_MAX_ELEMS_NAME TUNDRA_CONCAT3(TUNDRA_DYNSTK, TUNDRA_TYPENAME,\
    _MAX_ELEMS)

static const uint64 TUNDRA_MAX_ELEMS_NAME = 
    TUNDRA_UINT64_MAX / sizeof(TUNDRA_TYPE);

// Type and Function Name Macros -----------------------------------------------
#define TUNDRA_NAME TUNDRA_CONCAT(Tundra_DynamicStack, TUNDRA_TYPENAME)
#define TUNDRA_DYNARR_NAME TUNDRA_CONCAT(Tundra_DynamicArray, TUNDRA_TYPENAME)

#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynStk, TUNDRA_TYPENAME, \
    _##name)
#define TUNDRA_DYNARR_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, \
    TUNDRA_TYPENAME, _##name)
#define TUNDRA_INT_TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynStk, \
    TUNDRA_TYPENAME, _##name)

#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

typedef struct TUNDRA_NAME
{
    // Underlying DynamicArray used for storing elements.
    TUNDRA_DYNARR_NAME dyn_arr;

} TUNDRA_NAME;


// Internal Methods ------------------------------------------------------------


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes a Stack with default capacity. Allocates memory and 
 * sets internal components.
 *
 * Only initialize a Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 * 
 * @param stk Stack to init. 
*/
static inline void TUNDRA_FUNC_NAME(init)(TUNDRA_NAME *stk)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_cap)(&stk->dyn_arr, 
        TUNDRA_DYNSTK_DEF_CAP);
}

/**
 * @brief Initialize a Stack with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Stack is initialized with default capacity.
 *
 * Only initialize a Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 * 
 * @param stk Stack to init, 
 * @param init_cap Specified initial capacity.
 */
static inline void TUNDRA_FUNC_NAME(init_w_cap)(TUNDRA_NAME *stk, 
    uint64 init_cap)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_cap)(&stk->dyn_arr, init_cap);
}

/**
 * @brief Initializes a Stack with initial elems. Allocates memory and sets
 * internal components.
 * 
 * Only initialize a Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 *
 * @param stk Stack to init. 
 * @param elems Stack of elements to copy in.
 * @param num_elem Number of elements in `elems`.
 */
static inline void TUNDRA_FUNC_NAME(init_w_elem)(TUNDRA_NAME *stk, 
    const TUNDRA_TYPE *elems, uint64 num_elem)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_elems)(&stk->dyn_arr, elems, num_elem);
}

/**
 * @brief Initializes a Stack by deep copying another Stack. Allocates memory 
 * and sets internal components.
 *
 * `src` must be an initialized Stack, and `dst` must be uninitialized. 
 * 
 * @param src Stack to source from, must be initialized. 
 * @param dst Stack to deep copy to, must be uninitialized. 
 */
static inline void TUNDRA_FUNC_NAME(init_w_copy)(const TUNDRA_NAME *src, 
    TUNDRA_NAME *dst)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_copy)(&src->dyn_arr, &dst->dyn_arr);
}

static inline void TUNDRA_FUNC_NAME(init_w_move)(TUNDRA_NAME *src,
    TUNDRA_NAME *dst)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_move)(&src->dyn_arr, &dst->dyn_arr);
}
