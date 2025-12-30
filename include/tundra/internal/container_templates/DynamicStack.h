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

/**
 * @brief Initializes a Stack by transferring ownership of resources from 
 * another Stack. `src` is left in an uninitialized state.
 * 
 * `src` must be an initialized Stack, and `dst` must be uninitialized.
 * 
 * `src` will be left in an uninitialized state.
 * 
 * @param src Stack to source from, must be initialized.
 * @param dst Stack to transfer resources to, must be uninitialized.
 */
static inline void TUNDRA_FUNC_NAME(init_w_move)(TUNDRA_NAME *src,
    TUNDRA_NAME *dst)
{
    TUNDRA_DYNARR_FUNC_NAME(init_w_move)(&src->dyn_arr, &dst->dyn_arr);
}

/**
 * @brief Frees memory allocated for an initialized Stack.
 * 
 * After calling this method, the Stack must not be used unless reinitialized.
 * 
 * The user should not call this method on an uninitialized Stack.
 */
static inline void TUNDRA_FUNC_NAME(free)(TUNDRA_NAME *stk)
{
    TUNDRA_DYNARR_FUNC_NAME(free)(&stk->dyn_arr);
}

/**
 * @brief Deep copies `src` to `dst`. Both Stacks must be initialized.
 * 
 * If the Stacks are of the same address, nothing is done.
 * 
 * If the Stacks' capacity does not match, `dst` is reallocated to match `src`.
 * 
 * @param src Stack to source from.
 * @param dst Stack to deep copy to.
 */
static inline void TUNDRA_FUNC_NAME(copy)(const TUNDRA_NAME *src,
    TUNDRA_NAME *dst)
{
    TUNDRA_DYNARR_FUNC_NAME(copy)(&src->dyn_arr, &dst->dyn_arr);
}

/**
 * @brief Transfers ownership of resources from `src` to `dst. `src` is left 
 * in an uninitialized state. Both Stacks must be initialized.
 * 
 * If the Stacks are of the same address, nothing is done.
 * 
 * @param src Stack to source from.
 * @param dst Stack to transfer resources to.
 */
static inline void TUNDRA_FUNC_NAME(move)(TUNDRA_NAME *src, 
    TUNDRA_NAME *dst)
{
    TUNDRA_DYNARR_FUNC_NAME(move)(&src->dyn_arr, &dst->dyn_arr);
}

/**
 * @brief Clears the Stack of all elements. The capacity remains unchanged.
 * 
 * @param stk Stack to clear.
 */
static inline void TUNDRA_FUNC_NAME(clear)(TUNDRA_NAME *stk)
{
    TUNDRA_DYNARR_FUNC_NAME(free)(&stk->dyn_arr);
}

/**
 * @brief Adds a copy of an element to the top of the Stack, expanding if 
 * necessary.
 * 
 * @param stk Stack to add to.
 * @param elem Pointer to the element to add.
 */
static inline void TUNDRA_FUNC_NAME(push)(TUNDRA_NAME *stk, 
    const TUNDRA_TYPE *elem)
{
    TUNDRA_DYNARR_FUNC_NAME(add)(&stk->dyn_arr, elem);
}

/**
 * @brief Copies elements onto the Stack, expanding if needed.
 * 
 * Reserves memory beforehand, optimizing over individual adds.
 * 
 * The last element in `elems` will be the last element placed on the Stack, 
 * therefore will be on top and the first to pop off.
 * 
 */
static inline void TUNDRA_FUNC_NAME(push_multiple)(TUNDRA_NAME *stk,
    const TUNDRA_TYPE *elems, uint64 num_elem)
{
    TUNDRA_DYNARR_FUNC_NAME(add_multiple)(&stk->dyn_arr, elems, num_elem);
}

/**
 * @brief Resizes the Stack to contain `num_elem` elements.
 * 
 * Assumes that `num_elem` does not already match the number of elements in the 
 * Stack.
 * 
 * If `num_elem` is greater than the current capacity, the Stack's capacity is 
 * expanded exponentially to hold at least the new number of elements and the 
 * number of elements expands to `num_elem`. New elements are left 
 * untouched/uninitialized, but are indexable. Users are responsible for any 
 * initialization of the new elements.
 * 
 * If `num_elem` is less than the current number of elements, excess elements 
 * are discarded with the capacity remaining unchanged. If you wish to shrink 
 * the capacity, use `shrink_to_fit` or `shrink_to_new_cap`.
 * 
 * @param stk Stack to resize.
 * @param num_elem Number of elements to resize for.
 */
static inline void TUNDRA_FUNC_NAME(resize)(TUNDRA_NAME *stk, uint64 num_elem)
{
    TUNDRA_DYNARR_FUNC_NAME(resize)(&stk->dyn_arr, num_elem);
}

/**
 * @brief Expands the Stack to ensure it has the capacity to store 
 * `num_extra_elem` additional elems.
 * 
 * If the Stack already has enough capacity, nothing is done.
 * 
 * @param stk Array to reserve for. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void TUNDRA_FUNC_NAME(reserve)(TUNDRA_NAME *stk, 
    uint64 num_extra_elem)
{
    TUNDRA_DYNARR_FUNC_NAME(reserve)(&stk->dyn_arr, num_extra_elem);
}

/**
 * @brief Shrinks the Stack's allocated capacity to a specified capacity.
 *
 * If `new_cap` is greater than or equal to the current capacity, the Stack is
 * not modified. If `new_cap` is less than the current number of elems, 
 * excess elems are discarded and the Stack is resized to the value 
 * specified.
 *
 * Memory is reallocated if the capacity is reduced. 
 *
 * @param stk Stack to shrink. 
 * @param new_cap Capacity to shrink to.
 */
static inline void TUNDRA_FUNC_NAME(shrink_to_new_cap)(TUNDRA_NAME *stk,
    uint64 new_cap)
{
    TUNDRA_DYNARR_FUNC_NAME(shrink_to_new_cap)(&stk->dyn_arr, new_cap);
}

/**
 * @brief Shrinks the Stack's allocated capacity to the smallest power of 2 that
 * can hold its current number of elements.
 *
 * Memory is reallocated if capacity is not already the smallest power of 2.
 * 
 * @param stk Stack to shrink. 
 */
static inline void TUNDRA_FUNC_NAME(shrink_to_fit)(TUNDRA_NAME *stk)
{
    TUNDRA_DYNARR_FUNC_NAME(shrink_to_fit)(&stk->dyn_arr);
}

/**
 * @brief Removes the top element of the Stack. Does not return the removed 
 * value, use `front` to obtain its value beforehand.
 * 
 * A fatal is thrown if the Stack is empty with the Stack unmodified.
 * 
 * @param stk Stack to pop from.
 */
static inline void TUNDRA_FUNC_NAME(pop)(TUNDRA_NAME *stk)
{
    TUNDRA_DYNARR_FUNC_NAME(erase_back)(&stk->dyn_arr);
}

/**
 * @brief Returns a pointer to the top element of the Stack.
 *
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Stack to query.
 * 
 * @return TUNDRA_TYPE* Pointer to the first element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(TUNDRA_NAME *stk)
{
    return TUNDRA_DYNARR_FUNC_NAME(back)(&stk->dyn_arr);
}

/**
 * @brief Returns a const-pointer to the top element of the Stack.
 *
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Stack to query.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to the first element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_cst)(
    const TUNDRA_NAME *stk)
{
    return TUNDRA_DYNARR_FUNC_NAME(back_cst)(&stk->dyn_arr);
}

/**
 * @brief Returns true if the Stack is empty.
 * 
 * @param stk Stack to query.
 */
static inline bool TUNDRA_FUNC_NAME(is_empty)(const TUNDRA_NAME *stk)
{
    return TUNDRA_DYNARR_FUNC_NAME(size)(&stk->dyn_arr) == 0;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Stack to query.
 * 
 * @return uint64 Number of elements.
 */
static inline uint64 TUNDRA_FUNC_NAME(size)(const TUNDRA_NAME *stk)
{
    return TUNDRA_DYNARR_FUNC_NAME(size)(&stk->dyn_arr);
}

/**
 * @brief Returns the current capacity of the Stack.
 * 
 * @param stk Stack to query.
 * 
 * @return uint64 Current capacity.
 */
static inline uint64 TUNDRA_FUNC_NAME(capacity)(const TUNDRA_NAME *stk)
{
    return TUNDRA_DYNARR_FUNC_NAME(capacity)(&stk->dyn_arr);
}
