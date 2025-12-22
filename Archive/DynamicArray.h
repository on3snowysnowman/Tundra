/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elems.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

/**
 * To define a DynamicArray type, define the following macros as well as a 
 * header guard before including this file:
 * - TUNDRA_TYPE: Type the Array will store.
 * If these macros are not defined, they will default to:
 * - TUNDRA_TYPE: int
*/

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/Math.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/utils/Iterator.h"

#ifndef TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_H
#define TUNDRA_DYNARR_DEF_CAP 4
#endif

#ifndef TUNDRA_TYPE
#define TYPE int
#else 
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

#define NAME TUNDRA_CONCAT(Tundra_DynamicArray, TYPE)
#define ITER_NAME TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _Iter)
#define CST_ITER_NAME TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _CstIter)

#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, TYPE, _##name)
#define INT_ITER_FUNC_NAME(name) TUNDRA_CONCAT3(\
    InTundra_DynArr, TYPE, _It_##name)
#define INT_CST_ITER_FUNC_NAME(name) TUNDRA_CONCAT4(\
    InTundra_DynArr, TYPE, _CstIt_##name)


#ifdef __cplusplus
extern "C" {
#endif

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally 
 * added elems.
 *
 * Must be initialized using either the `init`, `copy` or `move` methods before 
 * use. Must be freed on end of use using the `free` method.
 *
 * Internals are read-only.
 */
typedef struct NAME
{
    // Heap allocated array of elems.
    TYPE *data; 

    // Number of elems currently added to the Array.
    uint64 num_elem;

    // Current maximum number of elems that can be added to the Array before 
    // it needs to be resized.
    uint64 cap;

    // Copy function invoked when copying individual elems.
    void (*per_object_copy_func)(const TYPE*, TYPE*);
    
    // Copy function invoked when a call to `copy` is made. If the user has not 
    // set a custom copy function, this defaults to a simple byte copy on the 
    // entire array. If a custom copy function is set, this function pointer 
    // will point to a function that instead iterates through each element 
    // individually and calls `per_object_copy_func` on each element.
    void (*copy_call_func)(const TYPE*, TYPE*, uint64, 
        void(*)(const TYPE*, TYPE*));

    // Free function invoked when freeing individual elems.
    void (*per_object_free_func)(TYPE*);

    // Same purpose as the `copy_call_func`, but for freeing elems.
    void (*free_call_func)(TYPE*, uint64, void(*)(TYPE*));
} NAME;

typedef struct ITER_NAME
{
    // Function pointer data for the iterator.
    InTundra_IterData it_data;

    // Pointer to the Array this iterator is associated with.
    NAME *arr_ptr; 

    // Index the iterator points at.
    uint64 index;
} ITER_NAME;



// Internal Methods ------------------------------------------------------------

/**
 * @brief Default copy method, performs simple byte copy on `num_elem` elems
 * from `src` to `dst`.
 * 
 * Used when the user has not set a custom copy function.
 * 
 * @param src Array of elems to source from.
 * @param dst Array of elems to copy to.
 * @param num_elem Number of elems to copy.
 */
static inline void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem, void (*per_object_copy_func)(const TYPE*, TYPE*))
{
    Tundra_copy_mem_fwd((const void*)src, (void*)dst, num_elem * sizeof(TYPE));
}


/**
 * @brief Custom copy method that iterates through each element and calls the 
 * user defined per-object copy function.
 * 
 * Used when the user has set a custom copy function.
 * 
 * @param src Array of elems to source from.
 * @param dst Array of elems to copy to.
 * @param num_elem Number of elems to copy.
 * @param per_object_copy_func User defined per-object copy function.
 */
static inline void INT_FUNC_NAME(custom_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem, void (*per_object_copy_func)(const TYPE*, TYPE*))
{
    // Iterate through each element, calling the custom user defined copy 
    // function.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        per_object_copy_func(src + i, dst + i);
    }
}

/**
 * @brief Default free method that simply frees the memory.
 * 
 * Used when the user has not set a custom free function.
 * 
 * @param mem Mem to free.
 * @param num_elem Number of elems in mem.
 * @param per_object_free_func User defined per-object free function.
 */
static inline void INT_FUNC_NAME(def_free)(TYPE *mem, uint64 num_elem,
    void (*per_object_free_func)(TYPE*))
{
    Tundra_free_mem((void*)mem);
}

/**
 * @brief Custom free method that iterates through each element and calls the 
 * user defined per-object free function.
 * 
 * Used when the user has set a custom free function.
 * 
 * @param mem Mem to free.
 * @param num_elem Number of elems in mem.
 * @param per_object_free_func User defined per-object free function.
 */
static inline void INT_FUNC_NAME(custom_free)(TYPE *mem, uint64 num_elem,
    void (*per_object_free_func)(TYPE*))
{
    // Iterate through each element, calling the custom user defined free 
    // function.
    for(uint64 i = 0; i < num_elem; ++i)
    {
        per_object_free_func(mem + i);
    }

    Tundra_free_mem((void*)mem);
}

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
    arr->per_object_copy_func = NULL;
    arr->copy_call_func = INT_FUNC_NAME(def_copy);
    arr->per_object_free_func = NULL;
    arr->free_call_func = INT_FUNC_NAME(def_free);
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

    TYPE *new_mem = (TYPE*)(Tundra_alloc_mem(NEW_CAP_ELEM * sizeof(TYPE)));

    // Copy data from the array to the new memory. Since we're just transferring
    // existing elems, we can do a simple byte copy.
    Tundra_copy_mem_fwd(
        (const void*)arr->data,
        (void*)new_mem,
        arr->num_elem * sizeof(TYPE)
    );

    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem; 
    arr->cap = NEW_CAP_ELEM;
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
    // bytes are reached.
    const uint64 NEW_CAP_BYTE = InTundra_calc_new_capacity_by_doubling(
        TOT_REQ_BYTE, arr->cap * sizeof(TYPE));

    // Allocate new memory and copy existing elems over.
    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(
        (const void*)arr->data,
        NEW_CAP_BYTE,
        arr->num_elem * sizeof(TYPE));

    // Free old memory.
    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem;
    arr->cap = NEW_CAP_BYTE / sizeof(TYPE);
}

/**
 * @brief Internal shrink method, reallocates the Array to a capacity of `cap`.
 * 
 * Does not touch the elements themselves beyond copying them to the new memory 
 * block. Any excess elements beyond the new capacity are discarded, and should
 * be properly handled by the caller before invoking this method.
 * 
 * @param arr Array to shrink. 
 * @param cap Capacity to shrink to.
 */
static inline void INT_FUNC_NAME(shrink)(NAME *arr, uint64 cap)
{
    const uint64 CAP_BYTE = cap * sizeof(TYPE);

    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(arr->data, CAP_BYTE, CAP_BYTE);
    
    Tundra_free_mem((void*)arr->data);
    arr->data = new_mem;
    arr->cap = cap;

    // If the capacity was shrunk smaller than the existing elems, update the
    // number of elems.
    arr->num_elem = Tundra_clamp_max_u64(arr->num_elem, arr->cap);
}

// Internal Iterator Methods ---------------------------------------------------

static inline void INT_ITER_FUNC_NAME(next)(void *iter)
{
    ITER_NAME *iter_cast = (ITER_NAME*)iter;
    ++iter_cast->index;
}

static inline void INT_ITER_FUNC_NAME(prev)(void *iter)
{
    ITER_NAME *iter_cast = (ITER_NAME*)iter;
    --iter_cast->index;
}

static inline void* INT_ITER_FUNC_NAME(deref)(const void *iter)
{
    const ITER_NAME *iter_cast = (const ITER_NAME*)iter;
    return iter_cast->arr_ptr->data + iter_cast->index;
}

#include <stdio.h>
static inline bool INT_ITER_FUNC_NAME(compare)(const void *first_iter, 
    const void *second_iter)
{
    const ITER_NAME *first_cast = (const ITER_NAME*)first_iter;
    const ITER_NAME *second_cast = (const ITER_NAME*)second_iter;

    return first_cast->index == second_cast->index;
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
    arr->per_object_copy_func = NULL;
    arr->copy_call_func = INT_FUNC_NAME(def_copy);
    arr->per_object_free_func = NULL;
    arr->free_call_func = INT_FUNC_NAME(def_free);

    const uint64 NUM_CPY_BYTE = num_elem * sizeof(TYPE);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        arr->data = (TYPE*)(Tundra_alloc_mem(NUM_CPY_BYTE));
        arr->copy_call_func(elems, arr->data, num_elem);
        arr->num_elem = num_elem;
        arr->cap = num_elem;
        return;
    }

    // -- Use the "reserving" method to alloc, which will generally alloc more 
    // space than is needed to prevent immediate expansion on next add call. --

    // Temp var for retrieving the capacity of the allocated block through the
    // reserve call. Capacity in bytes, not elems.
    uint64 temp_cap_bytes;

    Tundra_alloc_reserve_mem(
        (void**)(&arr->data), 
        &temp_cap_bytes, 
        NUM_CPY_BYTE);

    arr->copy_call_func(elems, arr->data, num_elem);

    arr->num_elem = num_elem;
    arr->cap = temp_cap_bytes / sizeof(TYPE);
}

/**
 * @brief Initializes an Array by deep copying another Array. Allocates memory 
 * and sets internal components.
 *
 * `src` must be an initialized Array, and `dst` must be uninitialized. 
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to, can be uninitialized. 
 */
static inline void FUNC_NAME(init_w_copy)(const NAME *src, NAME *dst)
{
    const uint64 SRC_CAP_BYTE = src->cap * sizeof(TYPE);

    *dst = *src; // Copy all members first.

    // Perform deep copy of data.
    dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
    src->copy_call_func(src->data, dst->data, src->num_elem);
}

/**
 * @brief Initializes an Array by transferring ownership of resources from 
 * another Array. `src` is left in an uninitialized state.
 *
 * `src` must be an initialized Array, and `dst` must be uninitialized. 
 * 
 * `src` will be left in an uninitialized state.
 * 
 * @param src Array to source from. 
 * @param dst Array to transfer resources to, can be uninitialized.
 */
static inline void FUNC_NAME(init_w_move)(NAME *src, NAME *dst)
{
    *dst = *src;

    src->data = NULL;
    src->per_object_copy_func = NULL;
    src->copy_call_func = NULL;
    src->per_object_free_func = NULL;
    src->free_call_func = NULL;
}

/**
 * @brief Sets the copy function used when copying elems.
 * 
 * @param arr Array to set copy function for.
 * @param copy_func Copy function to set.
 */
static inline void FUNC_NAME(set_copy_func)(NAME *arr, 
    void (*copy_func)(const TYPE*, TYPE*))
{
    arr->per_object_copy_func = copy_func;

    // Set the copy call function to the special function that calls the user
    // defined copy function on individual elems.
    arr->copy_call_func = INT_FUNC_NAME(custom_copy);
}

/**
 * @brief Sets the free function used when freeing elems.
 * 
 * @param arr Array to set free function for.
 * @param free_func Free function to set.
 */
static inline void FUNC_NAME(set_free_func)(NAME *arr, 
    void (*free_func)(NAME*))
{
    arr->per_object_free_func = free_func;;
    
    // Set the free call function to the special function that calls the user
    // defined free function on individual elems.
    arr->free_call_func = INT_FUNC_NAME(custom_free);
}

/**
 * @brief Frees memory allocated for an Array.
 *
 * After calling this method, the Array must not be used unless reinitialized.
 *
 * It is safe to call this method with an Array that has already been freed, or
 * never initialized.
 * 
 * @param arr Array to free.
 */
static inline void FUNC_NAME(free)(NAME *arr)
{
    arr->free_call_func(arr->data);
    arr->data = NULL;
    arr->per_object_copy_func = NULL;
    arr->copy_call_func = NULL;
    arr->per_object_free_func = NULL;
    arr->free_call_func = NULL;
}

/**
 * @brief Deep copies `src` to `dst`. Both Arrays must be initialized.
 *
 * If the Arrays are of the same address, nothing is done.
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to.
 */
static inline void FUNC_NAME(copy)(const NAME *src, NAME *dst)
{
    if(dst == src) { return; }
    
    const uint64 SRC_CAP_BYTE = src->cap * sizeof(TYPE);

    // Free existing memory in dst.
    dst->free_call_func(dst->data);
    
    // Initialize new memory block in dst that is the same capacity as src.
    dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
    dst->cap = src->cap;

    // Copy data from src to dst.
    src->copy_call_func(src->data, dst->data, src->num_elem);

    dst->num_elem = src->num_elem;
    dst->per_object_copy_func = src->per_object_copy_func;
    dst->copy_call_func = src->copy_call_func;
    dst->per_object_free_func = src->per_object_free_func;
    dst->free_call_func = src->free_call_func;

    // if(dst->data == NULL)
    // {
    //     dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
    //     dst->cap = src->cap;
    // }
    
    // // dst->data is not NULL, check if capacity matches to prevent unnecessary
    // // allocations.
    // else if(dst->cap != src->cap)
    // {
    //     // Capacities are different and dst->data is not NULL, free existing
    //     // memory and allocate a block of src's capacity.
    //     src->free_func(dst->data, dst->num_elem);
    //     dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
    //     dst->cap = src->cap;
    // }

    // src->copy_func(src->data, dst->data, src->num_elem);
    // dst->num_elem = src->num_elem;
    // dst->copy_func = src->copy_func;
    // dst->free_func = src->free_func;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.  Both Arrays must be initialized.
 *
 * If the Arrays are of the same address, nothing is done.
 * 
 * @param src Array to source from. 
 * @param dst Array to transfer resources to, can be uninitialized.
 */
static inline void FUNC_NAME(move)(NAME *src, NAME *dst)
{
    if(dst == src) { return; }

    dst->free_call_func(dst->data);
    *dst = *src;
    src->data = NULL;
    src->per_object_copy_func = NULL;
    src->copy_call_func = NULL;
    src->per_object_free_func = NULL;
    src->free_call_func = NULL;
}

/**
 * @brief Clears the Array of all elems.
 *
 * Does not modify, shrink, deallocate or zero out underlying memory. Only the 
 * element count is reset to zero so subsequent adds will overwrite previous 
 * data from the start of the Array.
 * 
 * @param arr Array to clear. 
 */
static inline void FUNC_NAME(clear)(NAME *arr)
{
    arr->num_elem = 0;
}

/**
 * @brief Adds a copy of `elem` to the back of the Array, automatically resizing 
 * if needed.
 *  
 * @param arr Array to add to.
 * @param elem Element to copy.
 */
static inline void FUNC_NAME(add)(NAME *arr, TYPE elem)
{
    InTundra_DynArrint_check_handle_exp(arr);
    arr->data[arr->num_elem++] = elem;
}

/**
 * @brief Copies multiple elems to the end of the Array, automatically 
 * resizing if needed.
 * 
 * Reserves memory beforehand, optimizing over individual adds.
 *
 * @param arr Array to add to.
 * @param elems Array of elems ot copy in.
 * @param num_elem Number of elems in `elems`.
 */
static inline void FUNC_NAME(add_multiple)(NAME *arr, const TYPE *elems, 
    uint64 num_elem)
{
    if(arr->cap - arr->num_elem < num_elem)
    {
        INT_FUNC_NAME(reserve_for)(arr, num_elem);
    }

    arr->copy_call_func(
        elems, 
        arr->data + arr->num_elem, 
        num_elem);

    arr->num_elem += num_elem;
}

/**
 * @brief Inserts an element at a position, shifting all elems ahead of it 
 * forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to insert into.
 * @param element Element to insert.
 * @param index Insert index.
 */
static inline void FUNC_NAME(insert)(NAME *arr, TYPE elem, uint64 index)
{
    if(index > arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    InTundra_DynArrint_check_handle_exp(arr);

    // Move elems at the index and after forward by one.
    Tundra_copy_mem_bwd(
        (const void*)(arr->data + index),
        (void*)(arr->data + index + 1),
        (arr->num_elem - index) * sizeof(TYPE));

    arr->data[index] = elem;
    ++arr->num_elem;
}

/**
 * @brief Resizes the Array to contain `num_elem` elems.
 * 
 * If `num_elem` is greater than the current capacity, the Array is expanded to
 * hold the new number of elems. If `num_elem` is less than the current 
 * number of elems, excess elems are discarded.
 * 
 * If `num_elem` is greater than the current number of elems, the new
 * elems are uninitialized.
 * 
 * @param arr Array to resize.
 * @param num_elem Number of elems to resize to.
 */
static inline void FUNC_NAME(resize)(NAME *arr, uint64 num_elem)
{
    if(num_elem <= arr->num_elem)
    {
        arr->num_elem = num_elem;
        return;
    }

    // -- num_elem > arr->num_elem --

    const uint64 NEW_CAP_ELEM = InTundra_calc_new_capacity_by_doubling(
        num_elem * sizeof(TYPE), arr->cap * sizeof(TYPE)) / sizeof(TYPE);

    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(
        (const void*)arr->data,
        NEW_CAP_ELEM * sizeof(TYPE),
        arr->num_elem * sizeof(TYPE));

    arr->free_call_func(arr);
    arr->data = new_mem;
    arr->cap = NEW_CAP_ELEM;

    arr->num_elem = num_elem;
}

/**
 * @brief Expands the Array to ensure it has the capacity to store `num_extra_elem`
 * additional elems.
 * 
 * If the Array already has enough capacity, nothing is done.
 * 
 * @param arr Array to reserve for. 
 * @param num_extra_elem Number of extra elems.
 */
static inline void FUNC_NAME(reserve)(NAME *arr, uint64 num_extra_elem)
{
    if(arr->num_elem + num_extra_elem <= arr->cap) { return; }

    INT_FUNC_NAME(reserve_for)(arr, num_extra_elem);
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
static inline void FUNC_NAME(shrink_to_new_cap)(NAME *arr, uint64 new_cap)
{
    if(new_cap >= arr->cap) { return; }

    INT_FUNC_NAME(shrink)(arr, new_cap);   
}

/**
 * @brief Shrinks the Array's allocated capacity to match its current number of 
 * elems.
 *
 * Memory is reallocated if capacity does not match current number of elems.
 * 
 * @param arr Array to shrink. 
 */
static inline void FUNC_NAME(shrink_to_fit)(NAME *arr)
{
    if(arr->num_elem == arr->cap) { return; }

    INT_FUNC_NAME(shrink)(arr, arr->num_elem);
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
static inline void FUNC_NAME(erase)(NAME *arr, uint64 index)
{
    if(index >= arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
        return;
    }

    // Shift elems after index back by one.
    Tundra_erase_shift_left(
        (void*)arr->data,
        index * sizeof(TYPE), 
        sizeof(TYPE), 
        arr->num_elem * sizeof(TYPE));

    --arr->num_elem;
}

/**
 * @brief Returns a pointer to the first element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array to query.
 * 
 * @return TYPE* Pointer to the first element.
 */
static inline TYPE* FUNC_NAME(front)(NAME *arr)
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
 * @return const TYPE* Const-pointer to the first element.
 */
static inline const TYPE* FUNC_NAME(front_cst)(const NAME *arr)
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
 * @return TYPE* Pointer to the last element.
 */
static inline const TYPE* FUNC_NAME(back)(const NAME *arr)
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
 * @return const TYPE* Const-pointer to the last element.
 */
static inline const TYPE* FUNC_NAME(back_cst)(const NAME *arr)
{
    return arr->data + (arr->num_elem - 1);
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
 * @return TYPE* Pointer to the element at `index`.
 */
static inline TYPE* FUNC_NAME(at_nocheck)(NAME *arr, uint64 index)
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
 * @return const TYPE* Const-pointer to the element at `index`.
 */
static inline const TYPE* FUNC_NAME(at_nocheck_cst)(const NAME *arr, 
    uint64 index)
{
    return &(arr->data[index]);
}

/**
 * @brief Returns a pointer to the element at `index` with bounds checking.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified. If 
 * the fatal returns, the return value of this method is not defined.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return TYPE* Pointer to the element at `index`.
 */
static inline TYPE* FUNC_NAME(at)(NAME *arr, uint64 index)
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
 * A fatal is thrown if the index is out of range with the Array unmodified. If 
 * the fatal returns, the return value of this method is not defined.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return const TYPE* Const-pointer to the element at `index`.
 */
static inline const TYPE* FUNC_NAME(at_cst)(const NAME *arr, uint64 index)
{
    if(index >= arr->num_elem)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, arr->num_elem);
    }

    return &(arr->data[index]);
}

/**
 * @brief Returns an iterator to the first element of the Array. 
 * 
 * If the Array is empty, this iterator must not be dereferenced.
 * 
 * @param arr Array to get iterator of.
 * 
 * @return Tundra_DynamicArrayTYPE_Iterator Iterator to the first element.
 */
static inline ITER_NAME FUNC_NAME(begin)(NAME *arr)
{
    ITER_NAME it;

    it.it_data.next = INT_ITER_FUNC_NAME(next);
    it.it_data.prev = INT_ITER_FUNC_NAME(prev);
    it.it_data.deref = INT_ITER_FUNC_NAME(deref);
    it.it_data.cmp = INT_ITER_FUNC_NAME(compare);

    it.arr_ptr = arr;
    it.index = 0;
    
    return it;
}

/**
 * @brief Returns an iterator to one past the last element of the Array.
 * 
 * @param arr Array to get iterator of.
 * 
 * @return Tundra_DynamicArrayTYPE_Iterator Iterator to one past the last 
 * element.
 */
static inline ITER_NAME FUNC_NAME(end)(NAME *arr)
{
    ITER_NAME it;

    it.it_data.next = INT_ITER_FUNC_NAME(next);
    it.it_data.prev = INT_ITER_FUNC_NAME(prev);
    it.it_data.deref = INT_ITER_FUNC_NAME(deref);
    it.it_data.cmp = INT_ITER_FUNC_NAME(compare);

    it.arr_ptr = arr;
    it.index = arr->num_elem;

    return it;
}

/**
 * @brief Returns the number of elems in the Array.
 * 
 * @param arr Array to query.
 * 
 * @return uint64 Number of elems.
 */
static inline uint64 FUNC_NAME(size)(const NAME *arr)
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
static inline uint64 FUNC_NAME(capacity)(const NAME *arr)
{
    return arr->cap;
}

#ifdef __cplusplus
} // extern "C"
#endif


#undef TYPE
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME