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
#include "tundra/utils/NumLimits.h"
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

static const uint64 TUNDRA_MAX_ELEMS_NAME = 
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
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally
 * added elements.
 * 
 * Must be initialized using either the `init`, `copy` or `move` methods before
 * use. Must be freed using the `free` method when no longer needed.
 * 
 * @note If the stored TUNDRA_TYPE requires special handling for copying or 
 * freeing, the user must set custom copy and/or free functions using the 
 * provided `set_copy_func` and `set_free_func` methods.
 * 
 * Internals are read-only.
 */
typedef struct TUNDRA_NAME
{
    // Pointer to heap allocated array of the elements.
    TUNDRA_TYPE *data;

    // Number of indexable elements currently added to the Array.
    uint64 num_elem;

    // Current number of elements that can be added to the Array before
    // resizing is required.
    uint64 cap;

    // Current allocated capacity in bytes. Do note that cap * 
    // sizeof(TUNDRA_TYPE) may be less than cap_bytes if TUNDRA_TYPE size is not 
    // a power of 2. Do not use cap_bytes to obtain knowledge of the capacity in 
    // elems, use `cap` instead.
    uint64 cap_bytes;
} TUNDRA_NAME;

/**
 * @brief Iterator for the DynamicArray.
 * 
 */
typedef struct TUNDRA_ITER_NAME
{
    // Pointer to the Array being iterated over.
    TUNDRA_NAME *array;

    // Current index in the Array.
    uint64 index;
} TUNDRA_ITER_NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Internal init method called by the public init methods. Allocates
 * initial memory for `init_cap` elems and sets container components.
 * 
 * @param arr Array to initialize.
 * @param init_cap Initial capacity in elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(init)(TUNDRA_NAME *arr, uint64 init_cap)
{   
    // Calculate initial capacity in bytes as the next power of 2.
    uint64 INIT_CAP_BYTE = Tundra_ceil_pow2(init_cap * sizeof(TUNDRA_TYPE));

    arr->data = (TUNDRA_TYPE*)Tundra_alloc_mem(INIT_CAP_BYTE);

    arr->num_elem = 0;
    arr->cap =  INIT_CAP_BYTE / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = INIT_CAP_BYTE;
}
#include <stdio.h>
/**
 * @brief Internal method to allocate new memory for the Array and move
 * existing elems to the new memory. Updates the internal components of the 
 * Array.
 * 
 * @param arr Array to reallocate.
 * @param new_cap_bytes New capacity in bytes.
 */
static inline void TUNDRA_INT_FUNC_NAME(alloc_move_mem)(TUNDRA_NAME *arr, 
    uint64 new_cap_bytes)
{
    TUNDRA_TYPE *new_mem = (TUNDRA_TYPE*)Tundra_alloc_mem(new_cap_bytes);

// Elements need custom move handling, cannot be simply byte copied.
#if TUNDRA_NEEDS_CUSTOM_MOVE

    // Call the custom move function on each element.
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i, new_mem + i);
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
 * @brief Expands the Array to ensure it has the capacity to store 
 * `num_extra_elem` additional elems.
 * 
 * Assumes that the current number of elems plus `num_extra_elem` exceeds the
 * current capacity.
 * 
 * @param arr Array to handle. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(reserve_for)(TUNDRA_NAME *arr, 
    uint64 num_extra_elem)
{
    if (num_extra_elem > TUNDRA_MAX_ELEMS_NAME - arr->num_elem) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    }


    const uint64 TOTAL_ELEM = arr->num_elem + num_extra_elem;

    if(TOTAL_ELEM > (TUNDRA_UINT64_MAX / sizeof(TUNDRA_TYPE))) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    
    }
    const uint64 TOT_REQ_BYTE = TOTAL_ELEM * sizeof(TUNDRA_TYPE);

    // Calculate new capacity as the next power of 2 that can hold the required
    // bytes.
    const uint64 NEW_CAP_BYTE = Tundra_ceil_pow2(TOT_REQ_BYTE);

    // No need to expand if capacity in bytes is already sufficient.
    if(NEW_CAP_BYTE == arr->cap_bytes)
    {
        return;
    }

    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(arr, NEW_CAP_BYTE);
}

/**
 * @brief Shrinks the Array's allocated capacity to a specified capacity.
 *
 * @param arr Array to shrink.
 * @param cap New capacity in elems.
 */
static inline void TUNDRA_INT_FUNC_NAME(shrink)(TUNDRA_NAME *arr, uint64 cap)
{
    const uint64 CAP_BYTES_POW2 = Tundra_ceil_pow2(cap * sizeof(TUNDRA_TYPE));

// If elements need custom free handling, any excess elements must be freed 
// first.
#if TUNDRA_NEEDS_CUSTOM_FREE

    if(cap < arr->num_elem)
    {

        // Free excess elements first.
        for(uint64 i = cap; i < arr->num_elem; ++i)
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
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * sets internal components.
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
static inline void TUNDRA_FUNC_NAME(init_w_cap)(TUNDRA_NAME *arr, 
    uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_DYNARR_DEF_CAP : init_cap; 

    TUNDRA_INT_FUNC_NAME(init)(arr, init_cap);
}

/**
 * @brief Initializes an Array with initial elems. Allocates memory and sets
 * internal components.
 * 
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elems Array of elements to copy in.
 * @param num_elem Number of elements in `elems`.
 */
static inline void TUNDRA_FUNC_NAME(init_w_elems)(TUNDRA_NAME *arr, 
    const TUNDRA_TYPE *elems, uint64 num_elem)
{
    // Use minimum power of 2 that can hold num_elem.
    const uint64 INIT_CAP_BYTE = Tundra_ceil_pow2(num_elem * 
        sizeof(TUNDRA_TYPE));
    
    arr->data = (TUNDRA_TYPE*)Tundra_alloc_mem(INIT_CAP_BYTE);

// Elements need custom copy handling, cannot be simply byte copied.
#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(elems + i, arr->data + i);
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
static inline void TUNDRA_FUNC_NAME(init_w_copy)(const TUNDRA_NAME *src, 
    TUNDRA_NAME *dst)
{
    *dst = *src;

    dst->data = (TUNDRA_TYPE*)Tundra_alloc_mem(src->cap_bytes);

// Elements need custom copy handling.
#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(src->data + i, dst->data + i);
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
 * `src` will be left in an uninitialized state.
 * 
 * @param src Array to source from, must be initialized. 
 * @param dst Array to transfer resources to, must be uninitialized.
 */
static inline void TUNDRA_FUNC_NAME(init_w_move)(TUNDRA_NAME *src, 
    TUNDRA_NAME *dst)
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
    for(uint64 i = 0; i < arr->num_elem; ++i)
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
 * If the Arrays' capacity does not match, `dst` is reallocated to match `src`.
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to.
 */
static inline void TUNDRA_FUNC_NAME(copy)(const TUNDRA_NAME *src, 
    TUNDRA_NAME *dst)
{
    if(dst == src) { return; }

    // If capacity bytes do not match, reallocate dst to match src.
    if(dst->cap_bytes != src->cap_bytes)
    {
        TUNDRA_FUNC_NAME(free)(dst);
        dst->data = (TUNDRA_TYPE*)Tundra_alloc_mem(src->cap_bytes);
        dst->cap = src->cap;
        dst->cap_bytes = src->cap_bytes;
    }

// Elements need custom copy handling.
#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < src->num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(src->data + i, dst->data + i);
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
static inline void TUNDRA_FUNC_NAME(move)(TUNDRA_NAME *src, TUNDRA_NAME *dst)
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
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_FREE_CALL_SIG(arr->data + i);
    }

#endif 

    arr->num_elem = 0;
}

/**
 * @brief Adds a copy of an element to the end of the Array, expanding if 
 * necessary.
 * 
 * @param arr Array to add to.
 * @param elem Pointer to the element to add.
 */
static inline void TUNDRA_FUNC_NAME(add)(TUNDRA_NAME *arr, 
    const TUNDRA_TYPE *elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

#if TUNDRA_NEEDS_CUSTOM_COPY

    TUNDRA_COPY_CALL_SIG(elem, arr->data + arr->num_elem);

#else

    arr->data[arr->num_elem] = *elem;

#endif

    ++arr->num_elem;
}

/**
 * @brief Copies multiple elements to the end of the Array, expanding if needed.
 * 
 * Reserves memory beforehand, optimizing over individual adds.
 *
 * @param arr Array to add to.
 * @param elems Array of elems ot copy in.
 * @param num_elem Number of elements in `elems`.
 */
static inline void TUNDRA_FUNC_NAME(add_multiple)(TUNDRA_NAME *arr, 
    const TUNDRA_TYPE *elems, uint64 num_elem)
{
    if(arr->cap - arr->num_elem < num_elem)
    {
        TUNDRA_INT_FUNC_NAME(reserve_for)(arr, num_elem);
    }

#if TUNDRA_NEEDS_CUSTOM_COPY

    // Call the custom copy function on each element.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        TUNDRA_COPY_CALL_SIG(elems + i, arr->data + arr->num_elem + i);
    }

#else 

    // Simple byte copy.
    Tundra_copy_mem_fwd
    (
        (const void*)elems,
        (void*)(arr->data + arr->num_elem),
        num_elem * sizeof(TUNDRA_TYPE)
    );
#endif

    arr->num_elem += num_elem;
}

/**
 * @brief Inserts a copy of an element at a position, shifting all elems ahead 
 * of it forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to insert into.
 * @param element Element to insert.
 * @param index Insert index.
 */
static inline void TUNDRA_FUNC_NAME(insert)(TUNDRA_NAME *arr, 
    const TUNDRA_TYPE *elem, uint64 index)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    TUNDRA_INT_FUNC_NAME(check_handle_exp)(arr);

    // Move elements at the index and after forward by one.

#if TUNDRA_NEEDS_CUSTOM_MOVE

    for(uint64 i = arr->num_elem; i > index; --i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i - 1, arr->data + i);
    }

#else

    Tundra_copy_mem_bwd(
        (const void*)(arr->data + index),
        (void*)(arr->data + index + 1),
        (arr->num_elem - index) * sizeof(TUNDRA_TYPE));
#endif

    // Insert new element.

#if TUNDRA_NEEDS_CUSTOM_COPY

    TUNDRA_COPY_CALL_SIG(elem, arr->data + index);

#else

    arr->data[index] = *elem;

#endif

    ++arr->num_elem;
}

/**
 * @brief Resizes the Array to contain `num_elem` elements.
 * 
 * Assumes that `num_elem` does not already match the number of elements in the 
 * Array.
 * 
 * If `num_elem` is greater than the current capacity, the Array's capacity is 
 * expanded exponentially to hold at least the new number of elements and the 
 * number of elements expands to `num_elem`. New elements are left 
 * untouched/uninitialized, but are indexable. Users are responsible for any 
 * initialization of the new elements.
 * 
 * If `num_elem` is less than the current number of elements, excess elements 
 * are discarded with the capacity remaining unchanged. If you wish to shrink 
 * the capacity, use `shrink_to_fit` or `shrink_to_new_cap`.
 * 
 * @param arr Array to resize.
 * @param num_elem Number of elements to resize for.
 */
static inline void TUNDRA_FUNC_NAME(resize)(TUNDRA_NAME *arr, uint64 num_elem)
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

        for(uint64 i = num_elem; i < arr->num_elem; ++i)
        {
            TUNDRA_FREE_CALL_SIG(arr->data + i);
        }

    #endif

        arr->num_elem = num_elem;
        return;
    }   

    // -- num_elem > arr->num_elem, growing the Array --

    // We have room for the total requested elements, no need to reallocate.
    if(num_elem <= arr->cap) 
    {
        arr->num_elem = num_elem;
        return;
    }

    // -- Need to reallocate. --

    const uint64 NEW_CAP_BYTE = 
        Tundra_ceil_pow2(num_elem * sizeof(TUNDRA_TYPE));

    TUNDRA_TYPE *new_mem = (TUNDRA_TYPE*)Tundra_alloc_mem(NEW_CAP_BYTE);

    // -- Move over existing elements.

#if TUNDRA_NEEDS_CUSTOM_MOVE

    // Call the custom move function on each existing element.
    for(uint64 i = 0; i < arr->num_elem; ++i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i, new_mem + i);
    }

#else

    Tundra_copy_mem_fwd(
        (const void*)arr->data,
        (void*)new_mem,
        arr->num_elem * sizeof(TUNDRA_TYPE)
    );

#endif

    arr->cap = NEW_CAP_BYTE / sizeof(TUNDRA_TYPE);
    arr->cap_bytes = NEW_CAP_BYTE;

    // Since we've moved existing elements, we don't need to worry about 
    // handling any custom freeing, since in that case the custom move call 
    // should have left the moved from elements in an uninitialized state.
    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem;

    arr->num_elem = num_elem;
}

/**
 * @brief Expands the Array to ensure it has the capacity to store 
 * `num_extra_elem` additional elems.
 * 
 * If the Array already has enough capacity, nothing is done.
 * 
 * @param arr Array to reserve for. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void TUNDRA_FUNC_NAME(reserve)(TUNDRA_NAME *arr, 
    uint64 num_extra_elem)
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
static inline void TUNDRA_FUNC_NAME(shrink_to_new_cap)(TUNDRA_NAME *arr, 
    uint64 new_cap)
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
static inline void TUNDRA_FUNC_NAME(shrink_to_fit)(TUNDRA_NAME *arr)
{
    TUNDRA_INT_FUNC_NAME(shrink)(arr, arr->num_elem);
}

/**
 * @brief Removes the element at the specified index and shifts subsequent
 * elems back by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to erase from.
 * @param index Index to erase.
 */
static inline void TUNDRA_FUNC_NAME(erase)(TUNDRA_NAME *arr, uint64 index)
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

    for(uint64 i = index; i < arr->num_elem - 1; ++i)
    {
        TUNDRA_MOVE_CALL_SIG(arr->data + i + 1, arr->data + i);
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
static inline void TUNDRA_FUNC_NAME(swap_and_pop)(TUNDRA_NAME *arr, 
    uint64 index)
{
    // Erasing the last element.
    if(index == arr->num_elem - 1)
    {
    
    #if TUNDRA_NEEDS_CUSTOM_FREE

        TUNDRA_FREE_CALL_SIG(arr->data + num_elem - 1);

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

    TUNDRA_MOVE_CALL_SIG(arr->data + arr->num_elem - 1, arr->data + index);

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
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_nocheck)(TUNDRA_NAME *arr, 
    uint64 index)
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
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_nocheck_cst)(
    const TUNDRA_NAME *arr, uint64 index)
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
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at)(TUNDRA_NAME *arr, uint64 index)
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
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_cst)(
    const TUNDRA_NAME *arr, uint64 index)
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
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(TUNDRA_NAME *arr)
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
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_cst)(
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
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(back)(TUNDRA_NAME *arr)
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
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(back_cst)(
    const TUNDRA_NAME *arr)
{
    return arr->data + (arr->num_elem - 1);
}

/**
 * @brief Returns the number of elements in the Array.
 * 
 * @param arr Array to query.
 * 
 * @return uint64 Number of elements.
 */
static inline uint64 TUNDRA_FUNC_NAME(size)(const TUNDRA_NAME *arr)
{
    return arr->num_elem;
}

/**
 * @brief Returns the current capacity of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return uint64 Current capacity.
 */
static inline uint64 TUNDRA_FUNC_NAME(capacity)(const TUNDRA_NAME *arr)
{
    return arr->cap;
}


// Iterator Methods ------------------------------------------------------------

/**
 * @brief Returns an iterator to the beginning of the Array.
 * 
 * @param arr Array to get iterator for.
 * 
 * @return TUNDRA_ITER_NAME Iterator to the beginning of the Array.
 */
static inline TUNDRA_ITER_NAME TUNDRA_ITER_FUNC_NAME(begin)(TUNDRA_NAME *arr)
{
    return (TUNDRA_ITER_NAME)
    {
        .array = arr,
        .index = 0
    };
}

/**
 * @brief Returns an iterator one past the last element of the Array.
 * 
 * This iterator must not be dereferenced.
 * 
 * @param arr Array to get iterator of.
 * 
 * @return TUNDRA_ITER_NAME Iterator to one past the last element.
 */
static inline TUNDRA_ITER_NAME TUNDRA_ITER_FUNC_NAME(end)(TUNDRA_NAME *arr)
{
    return (TUNDRA_ITER_NAME)
    {
        .array = arr,
        .index = arr->num_elem
    };
}

/**
 * @brief Returns true if both iterators point to the same index.
 * 
 * Assumes that the iterators come from the same Array. This means that if the
 * iterators are from different Arrays but have the same index, this method
 * returns true. Only compare iterators from the same Array.
 * 
 * @param a First iterator.
 * @param b Second iterator.
 * 
 * @return bool True if both iterators point to the same index.
 */
static inline bool TUNDRA_ITER_FUNC_NAME(compare)
    (const TUNDRA_ITER_NAME *first, const TUNDRA_ITER_NAME *second)
{
    return first->index == second->index;
}

/**
 * @brief Advances an iterator to the next index.
 * 
 * Does not check for going past the end iterator.
 * 
 * @param iter Iterator to advance.
 */
static inline void TUNDRA_ITER_FUNC_NAME(next)(TUNDRA_ITER_NAME *iter)
{
    ++(iter->index);
}

/**
 * @brief Moves an iterator to the previous index.
 * 
 * Does not check for going before the begin iterator.
 * 
 * @param iter Iterator to move back.
 */
static inline void TUNDRA_ITER_FUNC_NAME(prev)(TUNDRA_ITER_NAME *iter)
{
    --(iter->index);
}

/**
 * @brief Dereferences an iterator to get a pointer to the current element.
 * 
 * Does not check if the iterator is valid.
 * 
 * @param iter Iterator to dereference.
 * 
 * @return TUNDRA_TYPE* Pointer to the current element.
 */
static inline TUNDRA_TYPE* TUNDRA_ITER_FUNC_NAME(deref)(
    const TUNDRA_ITER_NAME *iter)
{
    return iter->array->data + iter->index;
}

/**
 * @brief Dereferences an iterator to get a const-pointer to the current
 * element.
 * 
 * Does not check if the iterator is valid.
 * 
 * @param iter Iterator to dereference.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to the current element.
 */
static inline const TUNDRA_TYPE* TUNDRA_ITER_FUNC_NAME(deref_cst)(
    const TUNDRA_ITER_NAME *iter)
{
    return iter->array->data + iter->index;
}

#ifdef __cplusplus
} // extern "C"
#endif


#undef TUNDRA_NAME
#undef TUNDRA_ITER_NAME
#undef TUNDRA_FUNC_NAME
#undef TUNDRA_INT_FUNC_NAME
#undef TUNDRA_ITER_FUNC_NAME
#undef TUNDRA_MAX_ELEMS_NAME
