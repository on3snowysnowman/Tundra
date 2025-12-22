/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elems.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/utils/CoreDef.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/Math.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_DEF_CAP 4
#endif

#ifndef TUNDRA_TYPE
#define TYPE int
#else
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

// Type and Function Name Macros -----------------------------------------------
#define NAME TUNDRA_CONCAT(Tundra_DynamicArray, TYPE)
#define ITER_NAME TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _Iter)
#define CST_ITER_NAME TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _CstIter)

#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, TYPE, _##name)
#define INT_ITER_FUNC_NAME(name) TUNDRA_CONCAT3(\
    InTundra_DynArr, TYPE, _It_##name)
#define INT_CST_ITER_FUNC_NAME(name) TUNDRA_CONCAT4(\
    InTundra_DynArr, TYPE, _CIt_##name)

#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally
 * added elements.
 * 
 * Must be initialized using either the `init`, `copy` or `move` methods before
 * use. Must be freed using the `free` method when no longer needed.
 * 
 * @note If the stored TYPE requires special handling for copying or freeing,
 * the user must set custom copy and/or free functions using the provided
 * `set_copy_func` and `set_free_func` methods.
 * 
 * Internals are read-only.
 */
typedef struct NAME
{
    // Pointer to heap allocated array of the elements.
    TYPE *data;

    // Number of indexable elements currently added to the Array.
    uint64 num_elem;

    // Current number of elements that can be added to the Array before
    // resizing is required.
    uint64 cap;
} NAME;

/**
 * @brief Iterator for the DynamicArray.
 * 
 */
typedef struct ITER_NAME
{
    // Pointer to the Array being iterated over.
    const NAME *const array;

    // Current index in the Array.
    uint64 current_index;
} ITER_NAME;

/**
 * @brief Constant iterator for the DynamicArray.
 
 */
typedef struct CST_ITER_NAME
{
    // Pointer to the Array being iterated over.
    const NAME *const array;

    // Current index in the Array.
    uint64 current_index;
} CST_ITER_NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Internal init method called by the public init methods. Allocates
 * initial memory for `init_cap` elems and sets container components.
 * 
 * @param arr Array to initialize.
 * @param init_cap Initial capacity in elems.
 */
static inline void INT_FUNC_NAME(init)(NAME *arr, uint64 init_cap)
{   
    arr->data = (TYPE*)Tundra_alloc_mem(init_cap * sizeof(TYPE));

    arr->num_elem = 0;
    arr->cap = init_cap;
}

/**
 * @brief Internal method to allocate new memory for the Array and move
 * existing elems to the new memory. Updates the internal components of the 
 * Array.
 * 
 * @param arr Array to reallocate.
 * @param new_cap New capacity in elems.
 */
static inline void INT_FUNC_NAME(alloc_move_mem)(NAME *arr, uint64 new_cap)
{
    TYPE *new_mem = (TYPE*)Tundra_alloc_mem(new_cap * sizeof(TYPE));

// Elements need custom move handling, cannot be simply byte copied.
#ifdef TUNDRA_NEEDS_CUSTOM_MOVE

    // Call the custom move function on each element.
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_MOVE_FUNC_SIG(arr->data + i, new_mem + i);
    }

// No custom move handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd(
        (const void*)arr->data,
        (void*)new_mem,
        arr->num_elem * sizeof(TYPE)
    );

#endif

    // Since we have covered any custom move handling, we don't need to worry 
    // about freeing the old memory in a special way, since the move behavior 
    // has left the old memory in an uninitialized state.
    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem;
    arr->cap = new_cap;
}

/**
 * @brief Checks if an Array has filled its allocated capacity, expanding and
 * reallocating if it has.
 * 
 * @param arr Array to handle. 
 */
static inline void INT_FUNC_NAME(check_handle_exp)(NAME *arr)
{
    if(arr->num_elem < arr->cap) { return; }

    // Double previous capacity.
    const uint64 NEW_CAP_ELEM = 2 * arr->cap;

    INT_FUNC_NAME(alloc_move_mem)(arr, NEW_CAP_ELEM);
}

/**
 * @brief Expands the Array to ensure it has the capacity to store 
 * `num_extra_elem` additional elems.
 * 
 * Assumes that the current number of elems plus `num_extra_elem` exceeds the
 * current capacity.
 * 
 * @param arr Array to handle. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void INT_FUNC_NAME(reserve_for)(NAME *arr, uint64 num_extra_elem)
{
    const uint64 TOT_REQ_BYTE = 
        (arr->num_elem + num_extra_elem) * sizeof(TYPE);

    // Calculate new capacity by doubling current capacity until the required
    // bytes can be held.
    const uint64 NEW_CAP_BYTE = InTundra_calc_new_capacity_by_doubling(
        TOT_REQ_BYTE, arr->cap * sizeof(TYPE));

    INT_FUNC_NAME(alloc_move_mem)(arr, NEW_CAP_BYTE / sizeof(TYPE));
}

/**
 * @brief Shrinks the Array's allocated capacity to a specified capacity.
 * 
 * Assumes that `cap` is less than the current capacity.
 *
 * @param arr Array to shrink.
 * @param cap New capacity in elems.
 */
static inline void INT_FUNC_NAME(shrink)(NAME *arr, uint64 cap)
{
// If elements need custom free handling, any excess elements must be freed 
// first.
#ifdef TUNDRA_NEEDS_CUSTOM_FREE

    if(cap >= arr->num_elem)
    {
        // No need to free any elements, just reallocate to smaller memory.
        INT_FUNC_NAME(alloc_move_mem)(arr, cap);
        return;
    }

    // Free excess elements first.
    for(uint64 i = cap; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_FUNC_SIG(arr->data + i);
    }

    INT_FUNC_NAME(alloc_move_mem)(arr, cap);

    // Update number of elems to new capacity since excess elems were removed.
    arr->num_elem = cap;


// No custom free handling needed, can simply reallocate.
#else

    INT_FUNC_NAME(alloc_move_mem)(arr, cap);

    arr->num_elem = Tundra_clamp_max_u64(arr->num_elem, arr->cap);

#endif
}


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
static inline void FUNC_NAME(init)(NAME *arr)
{
    INT_FUNC_NAME(init)(arr, TUNDRA_DYNARR_DEF_CAP);
}

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
static inline void FUNC_NAME(init_w_cap)(NAME *arr, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_DYNARR_DEF_CAP : init_cap; 

    INT_FUNC_NAME(init)(arr, init_cap);
}

/**
 * @brief Initializes an Array with initial elems. Allocates memory and sets
 * internal components.
 *
 * `elems` are copied into the Array. `num_elem` specifies the number of 
 * elems (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated for the Array. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 * 
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elems Array of elements to copy in.
 * @param num_elem Number of elements in `elems`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
static inline void FUNC_NAME(init_w_elems)(NAME *arr, const TYPE *elems, 
    uint64 num_elem, bool strict_alloc)
{
    // Determine allocation capacity. If strict_alloc is set, allocate exactly
    // enough for the number of elems. Otherwise, allocate by doubling to
    // reduce immediate reallocations on next add.
    const uint64 ALLOC_CAP_BYTE = strict_alloc ? num_elem * sizeof(TYPE): 
        Tundra_ceil_pow2(num_elem * sizeof(TYPE));

    arr->data = (TYPE*)Tundra_alloc_mem(ALLOC_CAP_BYTE);

// Elements need custom copy handling, cannot be simply byte copied.
#ifdef TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        TUNDRA_COPY_FUNC_SIG(elems + i, arr->data + i);
    }

// No custom copy handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd
    (
        (const void*)elems,
        (void*)arr->data,
        num_elem * sizeof(TYPE)
    );

#endif

    arr->num_elem = num_elem;
    arr->cap = ALLOC_CAP_BYTE / sizeof(TYPE);
}

/**
 * @brief Initializes an Array by deep copying another Array. Allocates memory 
 * and sets internal components.
 *
 * `src` must be an initialized Array, and `dst` must be uninitialized. 
 * 
 * @param src Array to source from, must be initialized. 
 * @param dst Array to deep copy to, must be uninitialized. 
 */
static inline void FUNC_NAME(init_w_copy)(const NAME *src, NAME *dst)
{
    const uint64 SRC_CAP_BYTE = src->cap * sizeof(TYPE);

    *dst = *src;

    dst->data = (TYPE*)Tundra_alloc_mem(SRC_CAP_BYTE);

// Elements need custom copy handling.
#ifdef TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_FUNC_SIG(src->data + i, dst->data + i);
    }

// No custom copy handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd
    (
        (const void*)src->data,
        (void*)dst->data,
        src->num_elem * sizeof(TYPE)
    );

#endif
}

/**
 * @brief Initializes an Array by transferring ownership of resources from 
 * another Array. `src` is left in an uninitialized state.
 *
 * `src` must be an initialized Array, and `dst` must be uninitialized. 
 * 
 * `src` will be left in an uninitialized state.
 * 
 * @param src Array to source from, must be initialized. 
 * @param dst Array to transfer resources to, must be uninitialized.
 */
static inline void FUNC_NAME(init_w_move)(NAME *src, NAME *dst)
{
    *dst = *src;

    src->data = NULL;
}

/**
 * @brief Frees memory allocated for an initialized Array.
 *
 * After calling this method, the Array must not be used unless reinitialized.
 *
 * If a custom free function has been set, calling `free` on an uninitialized 
 * Array is wasteful and can cause undefined behavior depending on the user's
 * implementation.
 * 
 * @param arr Initialized Array to free.
 */
static inline void FUNC_NAME(free)(NAME *arr)
{

// Elements need custom free handling.
#ifdef TUNDRA_NEEDS_CUSTOM_FREE

    // Call the custom free function on each element.
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_FUNC_SIG(arr->data + i);
    }

#endif

    Tundra_free_mem((void*)arr->data);

    arr->data = NULL;
}

/**
 * @brief Deep copies `src` to `dst`. Both Arrays must be initialized.
 *
 * If the Arrays are of the same address, nothing is done.
 * 
 * If the Arrays capacity does not match, `dst` is reallocated to match `src`.
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to.
 */
static inline void FUNC_NAME(copy)(const NAME *src, NAME *dst)
{
    if(dst == src) { return; }

    // If capacities do not match, reallocate dst to match src.
    if(dst->cap != src->cap)
    {
        FUNC_NAME(free)(dst);
        dst->data = (TYPE*)Tundra_alloc_mem(src->cap * sizeof(TYPE));
        dst->cap = src->cap;
    }

// Elements need custom copy handling.
#ifdef TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_FUNC_SIG(src->data + i, dst->data + i);
    }

// No custom copy handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd
    (
        (const void*)src->data,
        (void*)dst->data,
        src->num_elem * sizeof(TYPE)
    );

#endif

    dst->num_elem = src->num_elem;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state. Both Arrays must be initialized.
 *
 * If the Arrays are of the same address, nothing is done.
 * 
 * @param src Array to source from. 
 * @param dst Array to transfer resources to.
 */
static inline void FUNC_NAME(move)(NAME *src, NAME *dst)
{
    if(dst == src) { return; }

    FUNC_NAME(free)(dst);

    *dst = *src;

    src->data = NULL;
}

/**
 * @brief Clears the Array of all its elements. The capacity remains unchanged.
 * 
 * @param arr Array to clear.
 */
static inline void FUNC_NAME(clear)(NAME *arr)
{

// Elements need custom free handling.
#ifdef TUNDRA_NEEDS_CUSTOM_FREE

    // Call the custom free function on each element.
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_FUNC_SIG(arr->data + i);
    }

#endif 

    arr->num_elem = 0;
}

/**
 * @brief Adds an element to the end of the Array, expanding if necessary.
 * 
 * @param arr Array to add to.
 * @param elem Element to add.
 */
static inline void FUNC_NAME(add)(NAME *arr, TYPE *elem)
{
    INT_FUNC_NAME(check_handle_exp)(arr);

#ifdef TUNDRA_NEEDS_CUSTOM_COPY

    TUNDRA_COPY_FUNC_SIG(elem, arr->data + arr->num_elem);

#else

    arr->data[arr->num_elem] = *elem;

#endif

    ++arr->num_elem;
}

static inline void FUNC_NAME(add_multiple)(NAME *arr, const TYPE *elems,
    uint64 num_elem)
{
    if(arr->cap - arr->num_elem < num_elem)
    {
        INT_FUNC_NAME(reserve_for)(arr, num_elem);
    }

#ifdef TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        TUNDRA_COPY_FUNC_SIG(elems + i, arr->data + arr->num_elem + i);
    }
#else 

    Tundra_copy_mem_fwd
    (
        (const void*)elems,
        (void*)(arr->data + arr->num_elem),
        num_elem * sizeof(TYPE)
    );
#endif

    arr->num_elem += num_elem;
}



#ifdef __cplusplus
} // extern "C"
#endif