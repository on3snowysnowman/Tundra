/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elems.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/common/TypeDef.h"
#include "tundra/common/NumLimits.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/Math.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_DEF_CAP 4
#endif

#define TUNDRA_MAX_ELEMS_NAME TUNDRA_CONCAT3(TUNDRA_DYNARR, TUNDRA_TYPENAME, \
    _MAX_ELEMS)

static const u64 TUNDRA_MAX_ELEMS_NAME = 
    TUNDRA_UINT64_MAX / sizeof(TUNDRA_TYPE);

// Type and Function Name Macros -----------------------------------------------
#define TUNDRA_NAME TUNDRA_CONCAT(Tundra_DynamicArray, TUNDRA_TYPENAME)
#define TUNDRA_ITER_NAME TUNDRA_CONCAT(Tundra_DynamicArrayIterator, \
    TUNDRA_TYPENAME)

#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TUNDRA_TYPENAME, \
    _##name)
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, \
    TUNDRA_TYPENAME, _##name)
#define TUNDRA_ITER_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArrIter, \
    TUNDRA_TYPENAME, _##name)

    
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally
 * added elements.
 * 
 * Must be initialized using any of the `init` methods before use. Must be freed 
 * using the `free` method when no longer needed.
 * 
 * Internals are read-only.
 */
typedef struct TUNDRA_NAME
{
    // Pointer to heap allocated array of the elements.
    TUNDRA_TYPE *data;

    // Number of indexable elements currently added to the Array.
    u64 num_elem;

    // Current number of elements that can be added to the Array before
    // resizing is required.
    u64 cap;

    // Current allocated capacity in bytes. Do note that cap * 
    // sizeof(TUNDRA_TYPE) may be less than cap_bytes if TUNDRA_TYPE size is not 
    // a power of 2. 
    u64 cap_bytes;
} TUNDRA_NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Internal init method called by the public init methods. Allocates
 * initial memory for `init_cap` elems and sets container components.
 * 
 * @param arr Array to initialize.
 * @param init_cap Initial capacity in elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(init)(TUNDRA_NAME *arr, u64 init_cap)
{   
    // Calculate initial capacity in bytes as the next power of 2.
    const u64 INIT_CAP_BYTE = 
        Tundra_ceil_pow2(init_cap * sizeof(TUNDRA_TYPE));

    arr->data = (TUNDRA_TYPE*)Tundra_alloc_mem(INIT_CAP_BYTE);

    arr->num_elem = 0;
    arr->cap =  INIT_CAP_BYTE / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = INIT_CAP_BYTE;
}

/**
 * @brief Allocates new memory for the Array and moves existing elems to the new 
 * memory. Updates the internal components of the Array.
 * 
 * Expects `new_cap_bytes` to already be a power of 2.
 * 
 * @param arr Array to reallocate.
 * @param new_cap_bytes New capacity in bytes, power of 2.
 */
static inline void TUNDRA_INT_FUNC_NAME(alloc_move_mem)(TUNDRA_NAME *arr, 
    u64 new_cap_bytes)
{
    TUNDRA_TYPE *new_mem = (TUNDRA_TYPE*)Tundra_alloc_mem(new_cap_bytes);

// Elements need custom move handling, cannot be simply byte copied.
#if TUNDRA_NEEDS_CUSTOM_MOVE

    // Call the custom move function on each element.
    for(u64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_MOVE_CALL_SIG(new_mem + i, arr->data + i);
    }

// No custom move handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd(
        (const void*)arr->data,
        (void*)new_mem,
        arr->num_elem * sizeof(TUNDRA_TYPE)
    );

#endif

    // Since we have covered any custom move handling, we don't need to worry 
    // about freeing the old memory in a special way, since the move behavior 
    // has left the old memory in an uninitialized state.
    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem;
    arr->cap = new_cap_bytes / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = new_cap_bytes;
}

/**
 * @brief Checks if an Array has filled its allocated capacity, expanding and                              
 * reallocating if it has.
 * 
 * @param arr Array to handle. 
 */
static inline void TUNDRA_INT_FUNC_NAME(check_handle_exp)(TUNDRA_NAME *arr)
{
    if(arr->num_elem < arr->cap) { return; }

    // Prevent overflow.
    if(arr->cap_bytes > (TUNDRA_UINT64_MAX / 2)) 
    { 
        TUNDRA_FATAL("Capacity overflow on expansion."); 
    }

    // Double the capacity. Since the capacity in bytes is always a power of 2, 
    // doubling it will maintain that property.
    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(arr, 2 * arr->cap_bytes);
}

/**
 * @brief Called during an insert call, expands and reallocates an Array,
 * copying over bytes in a special way to prepare for an element to be inserted.                             
 * 
 * This function exists since when we're inserting an element, subsequent 
 * positions past the insert index need to be shifted to the right. Instead of
 * expanding, copying over the old bytes from the old Array memory, then 
 * shifting bytes to prepare an insert index, we can do that in one step. When
 * we allocate larger memory for the Array, we change the way we copy over the 
 * old bytes to leave the insert position open, then copy over the old bytes
 * past the index forward one, skipping redundant copying and shifting.
 * 
 * @param arr Array to handle.
 * @param index Index to prepare for insert.
 */
static inline void TUNDRA_INT_FUNC_NAME(handle_insert_exp)(TUNDRA_NAME *arr, 
    u64 index)
{
    const u64 NEW_CAP_BYTES = 2 * arr->cap_bytes;

    TUNDRA_TYPE *new_mem = (TUNDRA_TYPE *)Tundra_alloc_mem(NEW_CAP_BYTES);

    // Iterate over elements before the insert index.
    for(u64 i = 0; i < index; ++i)
    {
        #if TUNDRA_NEEDS_CUSTOM_MOVE

        TUNDRA_MOVE_CALL_SIG(new_mem + i, arr->data + i);
        #else

        new_mem[i] = arr->data[i];
        #endif
    }

    /// Iterate over elements starting at the insert index, placing them forward
    // by one to leave a spot open when the element will be inserted later 
    // (outside this function).
    for(u64 i = index; i < arr->num_elem; ++i)
    {
        #if TUNDRA_NEEDS_CUSTOM_MOVE

        TUNDRA_MOVE_CALL_SIG(new_mem + i + 1, arr->data + i);
        #else

        new_mem[i + 1] = arr->data[i];
        #endif
    }

    Tundra_free_mem((void *)arr->data);
    arr->data = new_mem;
    arr->cap = NEW_CAP_BYTES / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = NEW_CAP_BYTES;
}

/**
 * @brief Called by the public insert methods, handles expanding and shifting 
 * elements inside the Array to prepare for an element to be inserted at a 
 * position.
 * 
 * @param arr Array to handle.
 * @param index Index to prepare for insert.
 */
static inline void TUNDRA_INT_FUNC_NAME(prepare_insert)(TUNDRA_NAME *arr,
    u64 index)
{
    // If the Array does not have room for another element, need to realloc.
    if(arr->num_elem >= arr->cap) 
    { 
        // This method will prepare for an insert index during the expansion,
        // so no shifting past this call needs to be done.
        TUNDRA_INT_FUNC_NAME(handle_insert_exp)(arr, index);
        return;
    }

    // If the index is at the very end of the Array, we don't need to shift 
    // anything
    if(index == arr->num_elem) return;

    // -- arr->num_elem < arr->cap, there is room for one more element, just 
    // shift bytes at the insert index right one. --

    #if TUNDRA_NEEDS_CUSTOM_MOVE

    // Start shifting from the end since doing it from the beginning would 
    // overwrite the bytes we need to access before we can get to them.
    for(u64 i = arr->num_elem; i > index; --i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i, arr->data + i - 1);
    }
    #else

    Tundra_copy_mem_bwd(
        (const void *)(arr->data + index),
        (void *)(arr->data + index + 1),
        (arr->num_elem - index) * sizeof(TUNDRA_TYPE)
    );
    #endif
}

/**
 * @brief Expands the Array to ensure it has the capacity to store 
 * `num_extra_elem` additional elems.
 * 
 * Assumes that the current number of elems plus `num_extra_elem` exceeds the
 * current capacity.
 * 
 * @param arr Array to expand. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(reserve_for)(TUNDRA_NAME *arr, 
    u64 num_extra_elem)
{
    if (num_extra_elem > TUNDRA_MAX_ELEMS_NAME - arr->num_elem) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    }

    const u64 TOTAL_ELEM = arr->num_elem + num_extra_elem;

    if(TOTAL_ELEM > (TUNDRA_UINT64_MAX / sizeof(TUNDRA_TYPE))) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    
    }
    const u64 TOT_REQ_BYTE = TOTAL_ELEM * sizeof(TUNDRA_TYPE);

    // Calculate new capacity as the next power of 2 that can hold the required
    // bytes.
    const u64 NEW_CAP_BYTE = Tundra_ceil_pow2(TOT_REQ_BYTE);

    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(arr, NEW_CAP_BYTE);
}

/**
 * @brief Shrinks the Array's allocated capacity to a specified capacity.
 *
 * Capacity is rounded up to the nearest power of 2 capable of representing it.
 * 
 * @param arr Array to shrink.
 * @param cap New capacity in elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(shrink)(TUNDRA_NAME *arr, u64 cap)
{
    const u64 CAP_BYTES_POW2 = Tundra_ceil_pow2(cap * sizeof(TUNDRA_TYPE));

// If elements need custom free handling, any excess elements must be freed 
// first.
#if TUNDRA_NEEDS_CUSTOM_FREE

    if(cap < arr->num_elem)
    {

        // Free excess elements first.
        for(u64 i = cap; i < arr->num_elem; ++i)
        {
            TUNDRA_FREE_CALL_SIG(arr->data + i);
        }

        // Update number of elems to new capacity since excess elems were 
        // removed.
        arr->num_elem = cap;
    }

// No custom free handling needed, can simply reallocate.
#else

    arr->num_elem = Tundra_clamp_max_u64(arr->num_elem, cap);

#endif

    // No need to shrink if capacity in bytes is already the target.
    if(CAP_BYTES_POW2 == arr->cap_bytes)
    {
        return;
    }

    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(arr, CAP_BYTES_POW2);
}


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes an Array with default capacity. Allocates memory and sets 
 * internal components.
 *
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 * 
 * @param arr Array to init. 
*/
static inline void TUNDRA_FUNC_NAME(init)(TUNDRA_NAME *arr)
{
    TUNDRA_INT_FUNC_NAME(init)(arr, TUNDRA_DYNARR_DEF_CAP);
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
static inline void TUNDRA_FUNC_NAME(init_cap)(TUNDRA_NAME *arr, 
    u64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_DYNARR_DEF_CAP : init_cap; 

    TUNDRA_INT_FUNC_NAME(init)(arr, init_cap);
}

/**
 * @brief Initializes an Array with initial elems. Allocates memory and sets
 * internal components.
 * 
 * Each initial element is copied in.
 * 
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elems Array of elements to copy in.
 * @param num_elem Number of elements in `elems`.
 */
static inline void TUNDRA_FUNC_NAME(init_elems)(TUNDRA_NAME *arr, 
    const TUNDRA_TYPE *elems, u64 num_elem)
{
    // Use minimum power of 2 that can hold num_elem.
    const u64 INIT_CAP_BYTE = Tundra_ceil_pow2(num_elem * 
        sizeof(TUNDRA_TYPE));
    
    arr->data = (TUNDRA_TYPE*)Tundra_alloc_mem(INIT_CAP_BYTE);

    // Elements need custom copy handling, cannot be simply byte copied.
    #if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(u64 i = 0; i < num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(arr->data + i, elems + i);
    }

    // No custom copy handling needed, can do a byte copy.
    #else

    Tundra_copy_mem_fwd
    (
        (const void*)elems,
        (void*)arr->data,
        num_elem * sizeof(TUNDRA_TYPE)
    );

    #endif

    arr->num_elem = num_elem;
    arr->cap = INIT_CAP_BYTE / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = INIT_CAP_BYTE;
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
static inline void TUNDRA_FUNC_NAME(init_copy)(TUNDRA_NAME *dst, 
    const TUNDRA_NAME *src)
{
    // Shallow copy initially, we will deep copy required members next. 
    *dst = *src;

    dst->data = (TUNDRA_TYPE*)Tundra_alloc_mem(src->cap_bytes);

// Elements need custom copy handling.
#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(u64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(dst->data + i, src->data + i);
    }

// No custom copy handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd
    (
        (const void*)src->data,
        (void*)dst->data,
        src->num_elem * sizeof(TUNDRA_TYPE)
    );

#endif
}

/**
 * @brief Initializes an Array by transferring ownership of resources from 
 * another Array. `src` is left in an uninitialized state.
 *
 * `src` must be an initialized Array, and `dst` must be uninitialized. 
 * 
 * @param src Array to source from, must be initialized. 
 * @param dst Array to transfer resources to, must be uninitialized.
 */
static inline void TUNDRA_FUNC_NAME(init_move)(TUNDRA_NAME *dst, 
    TUNDRA_NAME *src)
{
    *dst = *src;

    src->data = NULL;
    src->num_elem = 0;
    src->cap = 0;
    src->cap_bytes = 0;
}

/**
 * @brief Frees memory allocated for an initialized Array.
 *
 * After calling this method, the Array must not be used unless reinitialized.
 *
 * The user should not call this method on an uninitialized Array. 
 * 
 * @param arr Initialized Array to free.
 */
static inline void TUNDRA_FUNC_NAME(free)(TUNDRA_NAME *arr)
{

// Elements need custom free handling.
#if TUNDRA_NEEDS_CUSTOM_FREE

    // Call the custom free function on each element.
    for(u64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_CALL_SIG(arr->data + i);
    }

#endif
    Tundra_free_mem((void*)arr->data);

    arr->data = NULL;
    arr->num_elem = 0;
    arr->cap = 0;
    arr->cap_bytes = 0;
}

/**
 * @brief Deep copies `src` to `dst`. Both Arrays must be initialized.
 *
 * If the Arrays are of the same address, nothing is done.
 * 
 * If the Arrays' capacities do not match, `dst` is reallocated to match `src`.
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to.
 */
static inline void TUNDRA_FUNC_NAME(assign_copy)(TUNDRA_NAME *dst, 
    const TUNDRA_NAME *src)
{
    if(dst == src) { return; }

    // If capacity bytes do not match, reallocate dst to match src.
    if(dst->cap_bytes != src->cap_bytes)
    {
        TUNDRA_FUNC_NAME(free)(dst);
        // Can simply use the init_copy method since dst is now uninitialized.
        TUNDRA_FUNC_NAME(init_copy)(dst, src);
        return;
    }

// Elements need custom copy handling.
#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(u64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(dst->data + i, src->data + i);
    }

// No custom copy handling needed, can do a byte copy.
#else

    Tundra_copy_mem_fwd
    (
        (const void*)src->data,
        (void*)dst->data,
        src->num_elem * sizeof(TUNDRA_TYPE)
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
static inline void TUNDRA_FUNC_NAME(assign_move)(TUNDRA_NAME *dst, 
    TUNDRA_NAME *src)
{
    if(dst == src) { return; }

    TUNDRA_FUNC_NAME(free)(dst);

    *dst = *src;

    src->data = NULL;
    src->num_elem = 0;
    src->cap = 0;
    src->cap_bytes = 0;
}

/**
 * @brief Clears the Array of all elements. The capacity remains unchanged.
 * 
 * @param arr Array to clear.
 */
static inline void TUNDRA_FUNC_NAME(clear)(TUNDRA_NAME *arr)
{

// Elements need custom free handling.
#if TUNDRA_NEEDS_CUSTOM_FREE

    // Call the custom free function on each element.
    for(u64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_CALL_SIG(arr->data + i);
    }

#endif 

    arr->num_elem = 0;
}

/**
 * @brief Adds an element to the end of the Array by copying it, expanding 
 * if necessary.
 * 
 * If a custom copy function is not defined for the element type, `elem` is 
 * simply byte copied.
 * 
 * `elem` cannot be a pointer inside the Array's memory. If the Array needs to 
 * expand and reallocate to add the element, previous memory is invalidated, 
 * including anything pointing to it.
 * 
 * @param arr Array to add to.
 * @param elem Pointer to the element to copy in.
 */
static inline void TUNDRA_FUNC_NAME(add_copy)(TUNDRA_NAME *arr,
    const TUNDRA_TYPE *elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

    #if TUNDRA_NEEDS_CUSTOM_COPY
    
    TUNDRA_COPY_CALL_SIG(arr->data + arr->num_elem, elem);
    #else

    arr->data[arr->num_elem] = *elem;
    #endif

    ++arr->num_elem;
}

/**
 * @brief Adds an element to the end of the Array by moving it, expanding if
 * necessary.
 * 
 * If a custom move function is not defined for the element type, `elem` is 
 * simply byte copied, and is not modified. In this case the behavior of this 
 * function is indistinguishable from the `add_copy` method as long as there 
 * is not a custom copy function defined.
 * 
 * `elem` cannot be a pointer inside the Array's memory. If the Array needs to 
 * expand and reallocate to add the element, previous memory is invalidated, 
 * including anything pointing to it.
 * 
 * @param arr Array to add to.
 * @param elem Pointer to the element to move in.
 */
static inline void TUNDRA_FUNC_NAME(add_move)(TUNDRA_NAME *arr, 
    TUNDRA_TYPE *elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

    #if TUNDRA_NEEDS_CUSTOM_MOVE

        TUNDRA_MOVE_CALL_SIG(arr->data + arr->num_elem, elem);
    #else

    arr->data[arr->num_elem] = *elem;
    #endif

    ++arr->num_elem;
}

static inline void TUNDRA_FUNC_NAME(add_val)(TUNDRA_NAME *arr,
    TUNDRA_TYPE elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

    arr->data[arr->num_elem] = elem;

    ++arr->num_elem;
}

static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(add_uninit)(TUNDRA_NAME *arr)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

    return arr->data + arr->num_elem++;
}

static inline void TUNDRA_FUNC_NAME(add_mult_copy)(TUNDRA_NAME *arr,
    const TUNDRA_TYPE *elems, u64 num_elem)
{
    TUNDRA_INT_FUNC_NAME(reserve_for)(arr, num_elem);

    #if TUNDRA_NEEDS_CUSTOM_COPY

    for(u64 i = 0; i < num_elem; ++i)
        TUNDRA_COPY_CALL_SIG(arr->data + arr->num_elem + i, elems[i]);

    #else

    Tundra_copy_mem_fwd(elems, arr->data + arr->num_elem, 
        num_elem * sizeof(TUNDRA_TYPE));

    #endif

    arr->num_elem = arr->num_elem + num_elem;
}

/**
 * @brief Inserts an element at an index by copying it, shifting all elements 
 * ahead of it forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * If a custom copy function is not defined for the element type, `elem` is 
 * simply byte copied.
 * 
 * `elem` cannot be a pointer inside the Array's memory. If the Array needs to 
 * expand and reallocate to add the element, previous memory is invalidated, 
 * including anything pointing to it.
 * 
 * @param arr Array to insert into.
 * @param index Insert index.
 * @param elem Pointer to the element to copy in.
 */
static inline void TUNDRA_FUNC_NAME(insert_copy)(TUNDRA_NAME *arr, 
    u64 index, const TUNDRA_TYPE *elem)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    TUNDRA_INT_FUNC_NAME(prepare_insert)(arr, index);

    // -- Insert new element. --

    #if TUNDRA_NEEDS_CUSTOM_COPY

    TUNDRA_COPY_CALL_SIG(arr->data + index, elem);
    #else

    arr->data[index] = *elem;
    #endif

    ++arr->num_elem;
}

/**
 * @brief Inserts an element at an index by moving it, shifting all elements
 * ahead of it forward by one.
 * 
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * If a custom move function is not defined for the element type, `elem` is 
 * simply byte copied, and is not modified. In this case the behavior of this 
 * function is indistinguishable from the `insert_copy` method as long 
 * as there is not a custom copy function defined.
 * 
 * `elem` cannot be a pointer inside the Array's memory. If the Array needs to 
 * expand and reallocate to add the element, previous memory is invalidated, 
 * including anything pointing to it.
 * 
 * @param arr Array to insert into.
 * @param index Insert index.
 * @param elem Pointer to the element to move in.
 */
static inline void TUNDRA_FUNC_NAME(insert_move)(TUNDRA_NAME *arr, 
    u64 index, TUNDRA_TYPE *elem)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    TUNDRA_INT_FUNC_NAME(prepare_insert)(arr, index);

    // -- Insert new element --

    #if TUNDRA_NEEDS_CUSTOM_MOVE

    TUNDRA_MOVE_CALL_SIG(arr->data + index, elem);
    #else

    arr->data[index] = *elem;
    #endif

    ++arr->num_elem;
}

static inline void TUNDRA_FUNC_NAME(insert_val)(TUNDRA_NAME *arr,
    u64 index, TUNDRA_TYPE elem)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    TUNDRA_INT_FUNC_NAME(prepare_insert)(arr, index);

    // -- Insert new element --

    #if TUNDRA_NEEDS_CUSTOM_MOVE

    TUNDRA_MOVE_CALL_SIG(arr->data + index, elem);
    #else

    arr->data[index] = elem;
    #endif

    ++arr->num_elem;
}

static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(insert_uninit)(TUNDRA_NAME *arr,
    u64 index)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return NULL;
    }

    TUNDRA_INT_FUNC_NAME(prepare_insert)(arr, index);

    return &arr->data[index];
}

/**
 * @brief Resizes the Array to contain `num_elem` elements.
 * 
 * Assumes that `num_elem` does not already match the number of elements in the 
 * Array.
 * 
 * If `num_elem` is greater than the current number of elements, new elements
 * are default initialized using the custom default init function if there is 
 * one. If there is not a custom function, the memory for the new elements are 
 * zeroed out. If the capacity is not sufficient to hold `num_elem`, the Array 
 * will allocate the smallest power of 2 capable of holding `num_elem`.
 * 
 * If `num_elem` is less than the current number of elements, excess elements 
 * are discarded with the capacity remaining unchanged. If you wish to shrink 
 * the capacity, use `shrink_fit` or `shrink_new_cap`.
 * 
 * @param arr Array to resize.
 * @param num_elem Number of elements to resize for.
 */
static inline void TUNDRA_FUNC_NAME(resize)(TUNDRA_NAME *arr, u64 num_elem)
{
    // Overflow check.
    if(num_elem > TUNDRA_MAX_ELEMS_NAME) 
    {
        TUNDRA_FATAL("Capacity overflow on resize.");
        return;
    }

    // Shrinking the number of elements.
    if(num_elem <= arr->num_elem)
    {
        // Trailing elements need custom free handling.
        #if TUNDRA_NEEDS_CUSTOM_FREE

        for(u64 i = num_elem; i < arr->num_elem; ++i)
        {
            TUNDRA_FREE_CALL_SIG(arr->data + i);
        }
        #endif

        arr->num_elem = num_elem;
        return;
    }   

    // -- num_elem > arr->num_elem, growing the Array --

    // If we need to reallocate.
    if(num_elem > arr->cap)
    {
        const u64 NEW_CAP_BYTES = 
            Tundra_ceil_pow2(num_elem * sizeof(TUNDRA_TYPE));

        TUNDRA_INT_FUNC_NAME(alloc_move_mem)(arr, NEW_CAP_BYTES);
    }

    // Default initialize any new elements.
    #if TUNDRA_NEEDS_CUSTOM_INIT
    
    for(u64 i = arr->num_elem; i < num_elem; ++i)
    {
        TUNDRA_DEF_INIT_CALL_SIG(arr->data + i);
    }
    #else

    Tundra_zero_out_mem((void *)(arr->data + arr->num_elem), 
        (num_elem - arr->num_elem) * sizeof(TUNDRA_TYPE));
    #endif

    arr->num_elem = num_elem;
}

/**
 * @brief Ensures the Array has the capacity to store `num_extra_elems`. If it 
 * does not, expands and reallocates the Array to hold the additional elements. 
 * 
 * The new expanded capacity will be the smallest power of 2 capable of holding 
 * the total needed elements.
 * 
 * @param arr Array reserve for.
 * @param num_extra_elem Number of extra elements.
 */
static inline void TUNDRA_FUNC_NAME(reserve)(TUNDRA_NAME *arr, 
    u64 num_extra_elem)
{
    if(arr->cap - arr->num_elem >= num_extra_elem) { return; }

    TUNDRA_INT_FUNC_NAME(reserve_for)(arr, num_extra_elem);
}

/**
 * @brief Shrinks the Array's allocated capacity to a specified capacity.
 *
 * If `new_cap` is greater than or equal to the current capacity, the Array is
 * not modified. If `new_cap` is less than the current number of elems, 
 * excess elems are discarded and the Array is resized to the value 
 * specified.
 *
 * Memory is reallocated if the capacity is reduced. 
 *
 * @param arr Array to shrink. 
 * @param new_cap Capacity to shrink to.
 */
static inline void TUNDRA_FUNC_NAME(shrink_new_cap)(TUNDRA_NAME *arr, 
    u64 new_cap)
{
    if(new_cap >= arr->cap) { return; }

    TUNDRA_INT_FUNC_NAME(shrink)(arr, new_cap);
}

/**
 * @brief Shrinks the Array's allocated capacity to the smallest power of 2 that
 * can hold its current number of elements.
 *
 * Memory is reallocated if capacity is not already the smallest power of 2.
 * 
 * @param arr Array to shrink. 
 */
static inline void TUNDRA_FUNC_NAME(shrink_fit)(TUNDRA_NAME *arr)
{
    TUNDRA_INT_FUNC_NAME(shrink)(arr, arr->num_elem);
}

/**
 * @brief Removes an element at an index and shifts subsequent elements back by 
 * one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to erase from.
 * @param index Index to erase.
 */
static inline void TUNDRA_FUNC_NAME(erase)(TUNDRA_NAME *arr, 
    u64 index)
{
    if(index >= arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index,  arr->num_elem);
        return;
    }

#if TUNDRA_NEEDS_CUSTOM_FREE

    TUNDRA_FREE_CALL_SIG(arr->data + index);

#endif

    // Shift elems after index back by one.
#if TUNDRA_NEEDS_CUSTOM_MOVE

    for(u64 i = index; i < arr->num_elem - 1; ++i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i, arr->data + i + 1);
    }

#else

    Tundra_copy_mem_fwd(
        (const void*)(arr->data + index + 1),
        (void*)(arr->data + index),
        (arr->num_elem - index - 1) * sizeof(TUNDRA_TYPE)
    );

#endif

    --arr->num_elem;
}

/**
 * @brief Removes the element at the end of the Array.
 * 
 * A fatal is thrown if the Array is empty with the Array unmodified.
 * 
 * @param arr Array to erase from.
 */
static inline void TUNDRA_FUNC_NAME(erase_back)(TUNDRA_NAME *arr)
{
    if(arr->num_elem == 0) 
    {
        TUNDRA_FATAL("Attempted to erase but the Array was empty.");
        return;
    }

#if TUNDRA_NEEDS_CUSTOM_FREE
    
    TUNDRA_FREE_CALL_SIG(arr->data + arr->num_elem - 1);

#endif

    --arr->num_elem;
}

/**
 * @brief Removes the element at the specified index by swapping it with the 
 * last element, then decreasing the size.
 *
 * This method provides O(1) removal time but does not preserve element order.
 * Use this method when the Array's order is not important.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to erase from.
 * @param index Index of the element to remove.
 */
static inline void TUNDRA_FUNC_NAME(swap_pop)(TUNDRA_NAME *arr, 
    u64 index)
{
    // Erasing the last element.
    if(index == arr->num_elem - 1)
    {
    
    #if TUNDRA_NEEDS_CUSTOM_FREE

        TUNDRA_FREE_CALL_SIG(arr->data + arr->num_elem - 1);

    #endif

        --arr->num_elem;
        return;
    }

    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index,  arr->num_elem);
        return;
    }

    // -- Move the last element to the erase position --

#if TUNDRA_NEEDS_CUSTOM_MOVE

    TUNDRA_MOVE_CALL_SIG(arr->data + index, arr->data + arr->num_elem - 1);

#else

    arr->data[index] = arr->data[arr->num_elem - 1]; 

#endif

    --arr->num_elem;
}

/**
 * @brief Returns a pointer to the element at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid. 
 * 
 * @param arr Array to index into.
 * @param index Index to get element.
 *
 * @return TUNDRA_TYPE* Pointer to the element at `index`.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_mut_nochk)(TUNDRA_NAME *arr, 
    u64 index)
{
    return &(arr->data[index]);
}

/**
 * @brief Returns a const-pointer to the element at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid. 
 * 
 * @param arr Array to index into.
 * @param index Index to get element.
 *
 * @return const TUNDRA_TYPE* Const-pointer to the element at `index`.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_nochk)(
    const TUNDRA_NAME *arr, u64 index)
{
    return &(arr->data[index]);
}

/**
 * @brief Returns a pointer to the element at `index` with bounds checking.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified,
 * returning NULL.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return TUNDRA_TYPE* Pointer to the element at `index`.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_mut)(const TUNDRA_NAME *arr, 
    u64 index)
{
    if(index >= arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
    }

    return &(arr->data[index]);
}

/**
 * @brief Returns a const-pointer to the element at `index` with bounds 
 * checking.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified,
 * returning NULL.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return const TUNDRA_TYPE* Const-pointer to the element at `index`.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at)(
    const TUNDRA_NAME *arr, u64 index)
{
    if(index >= arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
    }

    return &(arr->data[index]);
}

/**
 * @brief Returns a pointer to the first element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array to query.
 * 
 * @return TUNDRA_TYPE* Pointer to the first element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_mut)(TUNDRA_NAME *arr)
{
    return arr->data;
}

/**
 * @brief Returns a const-pointer to the first element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array to query.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to the first element.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(
    const TUNDRA_NAME *arr)
{
    return arr->data;
}

/**
 * @brief Returns a pointer to the last element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array to query.
 * 
 * @return TUNDRA_TYPE* Pointer to the last element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(back_mut)(TUNDRA_NAME *arr)
{
    return arr->data + (arr->num_elem - 1);
}

/**
 * @brief Returns a const-pointer to the last element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array to query.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to the last element.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(back)(
    const TUNDRA_NAME *arr)
{
    return arr->data + (arr->num_elem - 1);
}

static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(data)(const TUNDRA_NAME *arr)
{
    return arr->data;
}

/**
 * @brief Returns the number of elements in the Array.
 * 
 * @param arr Array to query.
 * 
 * @return u64 Number of elements.
 */
static inline u64 TUNDRA_FUNC_NAME(size)(const TUNDRA_NAME *arr)
{
    return arr->num_elem;
}

/**
 * @brief Returns the current capacity of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return u64 Current capacity.
 */
static inline u64 TUNDRA_FUNC_NAME(capacity)(const TUNDRA_NAME *arr)
{
    return arr->cap;
}


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#undef TUNDRA_MAX_ELEMS_NAME
#undef TUNDRA_NAME
#undef TUNDRA_ITER_NAME
#undef TUNDRA_FUNC_NAME
#undef TUNDRA_INT_FUNC_NAME
#undef TUNDRA_ITER_FUNC_NAME
