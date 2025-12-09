/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elements.
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
#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif

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
typedef struct NAME
{
    // Heap allocated array of elements.
    TYPE *data; 

    // Number of elements currently added to the Array.
    uint64 num_elem;

    // Current maximum number of elements that can be added to the Array before 
    // it needs to be resized.
    uint64 cap;

    // Copy function invoked when the `copy` method is called. 
    void (*copy_func)(const TYPE*, TYPE*, uint64);
    
    // Free function invoked when the `free` method is called.
    void (*free_func)(TYPE*, uint64);
} NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Default copy method, performs simple byte copy on `num_elem` elements
 * from `src` to `dst`.
 * 
 * @param src Array of elements to source from.
 * @param dst Array of elements to copy to.
 * @param num_elem Number of elements to copy.
 */
static inline void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem)
{
    Tundra_copy_mem_fwd((const void*)src, (void*)dst, num_elem * sizeof(TYPE));
}

/**
 * @brief Default free method that simply frees the memory.
 * 
 * @param mem Mem to free.
 */
static inline void INT_FUNC_NAME(def_free)(TYPE *mem, uint64 /** num_elem */ )
{
    Tundra_free_mem((void*)mem);
}

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` elements and sets container components.
 * 
 * @param arr Array to initialize. 
 * @param init_cap Initial capacity in elements.
 */
static inline void INT_FUNC_NAME(init)(NAME *arr, uint64 init_cap)
{
    arr->data = (TYPE*)Tundra_alloc_mem(init_cap * sizeof(TYPE));
    
    arr->num_elem = 0;
    arr->cap = init_cap;
    arr->copy_func = INT_FUNC_NAME(def_copy);
    arr->free_func = INT_FUNC_NAME(def_free);
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

    // Copy data from the array to the new memory.
    arr->copy_func(arr->data, new_mem, arr->num_elem);
    arr->free_func(arr->data, arr->num_elem);
    arr->data = new_mem; 
    arr->cap = NEW_CAP_ELEM;
}

/**
 * @brief Expands the Array to ensure it has the capacity to store 
 * `num_extra_elem` additional elements.
 * 
 * Assumes that the current number of elements plus `num_extra_elem` exceeds the
 * current capacity.
 * 
 * @param arr Array to handle. 
 * @param num_extra_elem Number of extra elements.
 */
static inline void INT_FUNC_NAME(reserve_for)(NAME *arr, uint64 num_extra_elem)
{
    const uint64 TOT_REQ_BYTE = 
        (arr->num_elem + num_extra_elem) * sizeof(TYPE);
    
    // Calculate new capacity by doubling current capacity until the required
    // bytes are reached.
    const uint64 NEW_CAP_BYTE = InTundra_calc_new_capacity_by_doubling(
        TOT_REQ_BYTE, arr->cap * sizeof(TYPE));

    // Allocate new memory and copy existing elements over.
    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(
        (const void*)arr->data,
        NEW_CAP_BYTE,
        arr->num_elem * sizeof(TYPE));

    // Free old memory.
    arr->free_func(arr->data, arr->num_elem);
    arr->data = new_mem;
    arr->cap = NEW_CAP_BYTE / sizeof(TYPE);
}

/**
 * @brief Internal shrink method, reallocates the Array to a capacity of `cap`.
 * 
 * @param arr Array to shrink. 
 * @param cap Capacity to shrink to.
 */
static inline void INT_FUNC_NAME(shrink)(NAME *arr, uint64 cap)
{
    const uint64 CAP_BYTE = cap * sizeof(TYPE);

    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(arr->data, CAP_BYTE, CAP_BYTE);

    // free_mem(arr->data);
    arr->free_func(arr->data, arr->num_elem);
    arr->data = new_mem;
    arr->cap = cap;

    // If the capacity was shrunk smaller than the existing elements, update the
    // number of elements.
    arr->num_elem = Tundra_clamp_max_u64(arr->num_elem, arr->cap);
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
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
static inline void FUNC_NAME(init_w_elems)(NAME *arr, const TYPE *elements, 
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

    arr->copy_func(elements, arr->data, num_elem);

    arr->num_elem = num_elem;
    arr->cap = temp_cap_bytes / sizeof(TYPE);
}

/**
 * @brief Sets the copy function used when copying elements.
 * 
 * @param arr Array to set copy function for.
 * @param copy_func Copy function to set.
 */
static inline void FUNC_NAME(set_copy_func)(NAME *arr, 
    void (*copy_func)(const TYPE*, TYPE*, uint64))
{
    arr->copy_func = copy_func;
}

/**
 * @brief Sets the free function used when freeing elements.
 * 
 * @param arr Array to set free function for.
 * @param free_func Free function to set.
 */
static inline void FUNC_NAME(set_free_func)(NAME *arr, 
    void (*free_func)(TYPE*, uint64))
{
    arr->free_func = free_func;
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
    arr->free_func(arr->data, arr->num_elem);
    arr->data = NULL;
    arr->copy_func = NULL;
    arr->free_func = NULL;
}

/**
 * @brief Deep copies `src` to `dst`.
 *
 * If the Arrays are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Array. 
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to, can be uninitialized. 
 */
static inline void FUNC_NAME(copy)(const NAME *src, NAME *dst)
{
    if(dst == src) { return; }
    
    const uint64 SRC_CAP_BYTE = src->cap * sizeof(TYPE);

    // TODO: Fix copy functions to support uninitialized dst properly, right now
    // it checks for NULL data pointer only, which is not guaranteed to be the 
    // the case for an uninitialized Array.

    if(dst->data == NULL)
    {
        dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
        dst->cap = src->cap;
    }
    
    // dst->data is not NULL, check if capacity matches to prevent unnecessary
    // allocations.
    else if(dst->cap != src->cap)
    {
        // Capacities are different and dst->data is not NULL, free existing
        // memory and allocate a block of src's capacity.
        src->free_func(dst->data, dst->num_elem);
        dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
        dst->cap = src->cap;
    }

    src->copy_func(src->data, dst->data, src->num_elem);
    dst->num_elem = src->num_elem;
    dst->copy_func = src->copy_func;
    dst->free_func = src->free_func;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.
 *
 * If the Arrays are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Array.
 * 
 * @param src Array to source from. 
 * @param dst Array to transfer resources to, can be uninitialized.
 */
static inline void FUNC_NAME(move)(NAME *src, NAME *dst)
{
    if(dst == src) { return; }

    src->free_func(dst->data, dst->num_elem);
    *dst = *src;
    src->data = NULL;
    src->copy_func = NULL;
    src->free_func = NULL;
}

/**
 * @brief Clears the Array of all elements.
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
 * @brief Copies multiple elements to the end of the Array, automatically 
 * resizing if needed.
 * 
 * Reserves memory beforehand, optimizing over individual adds.
 *
 * @param arr Array to add to.
 * @param elements Array of elements ot copy in.
 * @param num_elem Number of elements in `elements`.
 */
static inline void FUNC_NAME(add_multiple)(NAME *arr, const TYPE *elems, 
    uint64 num_elem)
{
    if(arr->cap - arr->num_elem < num_elem)
    {
        INT_FUNC_NAME(reserve_for)(arr, num_elem);
    }

    arr->copy_func(
        elems, 
        arr->data + arr->num_elem, 
        num_elem);

    arr->num_elem += num_elem;
}

/**
 * @brief Inserts an element at a position, shifting all elements ahead of it 
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

    // Move elements at the index and after forward by one.
    Tundra_copy_mem_bwd(
        (const void*)(arr->data + index),
        (void*)(arr->data + index + 1),
        (arr->num_elem - index) * sizeof(TYPE));

    arr->data[index] = elem;
    ++arr->num_elem;
}

/**
 * @brief Resizes the Array to contain `num_elem` elements.
 * 
 * If `num_elem` is greater than the current capacity, the Array is expanded to
 * hold the new number of elements. If `num_elem` is less than the current 
 * number of elements, excess elements are discarded.
 * 
 * If `num_elem` is greater than the current number of elements, the new
 * elements are uninitialized.
 * 
 * @param arr Array to resize.
 * @param num_elem Number of elements to resize to.
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

    arr->free_func(arr->data, arr->num_elem);
    arr->data = new_mem;
    arr->cap = NEW_CAP_ELEM;

    arr->num_elem = num_elem;
}

/**
 * @brief Expands the Array to ensure it has the capacity to store `num_extra_elem`
 * additional elements.
 * 
 * If the Array already has enough capacity, nothing is done.
 * 
 * @param arr Array to reserve for. 
 * @param num_extra_elem Number of extra elements.
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
 * not modified. If `new_cap` is less than the current number of elements, 
 * excess elements are discarded and the Array is resized to the value 
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
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
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
 * elements back by one.
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

    // Shift elements after index back by one.
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
 * @brief Returns the number of elements in the Array.
 * 
 * @param arr Array to query.
 * 
 * @return uint64 Number of elements.
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