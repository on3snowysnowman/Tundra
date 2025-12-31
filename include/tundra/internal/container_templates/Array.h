/**
 * @file Array.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed sized contiguous Array for storing elements.
 * @date 2025-12-05
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_ARRAY_H
#define TUNDRA_ARRAY_H
#define TUNDRA_ARR_DEF_CAP 4
#define TUNDRA_MAKE_ARRAY(...) \
    { .data = {__VA_ARGS__} }
#endif

#define TUNDRA_NAME TUNDRA_CONCAT3(Tundra_Array, TUNDRA_CAPACITY, \
    TUNDRA_EXPAND(TUNDRA_TYPE))
#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Arr, TUNDRA_CAPACITY, \
    TUNDRA_TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size stack allocated contiguous container for storing elements.
 * 
 * Does not require initialization or free calls.
 * 
 * Use TUNDRA_MAKE_ARRAY to create a Tundra_Array during compile time. For 
 * example, for an Array of type int and capacity 4:
 * 
 * Tundra_Array4int arr = TUNDRA_MAKE_ARRAY(1, 2, 3, 4);
 * 
 * If an Array is not created using TUNDRA_MAKE_ARRAY and is simply defined as
 * "Tundra_Array4int arr;", the Array will be in a valid, accessible state but 
 * will hold junk values, similar to creating a C array on the stack.
 * 
 * Unlike the other containers, the TundraArray does not handle custom element 
 * handling (copy, free, move). It simply acts as a wrapper around a C array to
 * provide convenient methods like size and constant indexing with bounds 
 * checks. Any custom handling like calling a custom copy or move method on an 
 * element should be handled by the user through external calls with the 
 * returned
 */
typedef struct TUNDRA_NAME
{
    // Stack allocated array of elements.
    TUNDRA_TYPE data[TUNDRA_CAPACITY];
} TUNDRA_NAME;


// Public Methods --------------------------------------------------------------

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
    return arr->data + index;
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
    return arr->data + index;
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
    if(index >= TUNDRA_CAPACITY)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            TUNDRA_CAPACITY);
        return NULL;
    }

    return arr->data + index;
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
    if(index >= TUNDRA_CAPACITY)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            TUNDRA_CAPACITY);
        return NULL;
    }

    return arr->data + index;
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
    return &arr->data[0];
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
    return arr->data + TUNDRA_CAPACITY - 1;
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
    return arr->data + TUNDRA_CAPACITY - 1;
}

/**
 * @brief Returns the fixed size of the Array type.
 *
 * @return uint64 Capacity.
 */
static inline uint64 TUNDRA_FUNC_NAME(size)()
{
    return TUNDRA_CAPACITY;
}


#ifdef __cplusplus
} // extern "C"
#endif  

#undef TUNDRA_NAME
#undef TUNDRA_FUNC_NAME
