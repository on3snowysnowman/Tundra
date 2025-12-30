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
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_Arr, \
    TUNDRA_CAPACITY, TUNDRA_TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size contiguous container for storing elements.
 * 
 * Stack allocated array with a fixed capacity. This container requires no
 * initialization or cleanup as all memory is managed on the stack.
 */
typedef struct TUNDRA_NAME
{
    TUNDRA_TYPE data[TUNDRA_CAPACITY];
} TUNDRA_NAME;


// Public Methods --------------------------------------------------------------

/**
 * @brief Returns a pointer to the value at an index with index checking.
 *
 * Performs a bounds check on `index`. If `index` is out of range, calls the
 * configured fatal handler. If the handler returns, behavior is undefined.
 * 
 * @param arr Array to index into.
 * @param index Index.
 *
 * @return TUNDRA_TYPE* Pointer to the item at the index. 
 */ 
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at)(TUNDRA_NAME *arr, uint64 index)
{
    if(index >= TUNDRA_CAPACITY)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            TUNDRA_CAPACITY);
    }

    return arr->data + index;;
}

/**
 * @brief Returns a const-pointer to the value at an index with index checking.
 *
 * Performs a bounds check on `index`. If `index` is out of range, calls the
 * configured fatal handler. If the handler returns, behavior is undefined.
 * 
 * @param arr Array to index into.
 * @param index Index.
 *
 * @return const TUNDRA_TYPE* Const-pointer to the item at the index. 
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_cst)(
    const TUNDRA_NAME *arr, uint64 index)
{
    if(index >= TUNDRA_CAPACITY)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            TUNDRA_CAPACITY);
    }

    return arr->data + index;
}

/**
 * @brief Returns a pointer to the value at an index without index checking.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid or 
 * undefined behavior will occur.
 *
 * @param arr Array to index into.
 * @param index Index.
 *
 * @return TUNDRA_TYPE* Pointer to the item at the index. 
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_nocheck)(TUNDRA_NAME *arr, 
    uint64 index)
{
    return arr->data + index;
}

/**
 * @brief Returns a const-pointer to the value at an index without index 
 * checking.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid or 
 * undefined behavior will occur.
 *
 * @param arr Array to index into.
 * @param index Index.
 *
 * @return const TUNDRA_TYPE* Const-pointer to the item at the index. 
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_nocheck_cst)(
    const TUNDRA_NAME *arr, uint64 index)
{
    return arr->data + index;
}

/**
 * @brief Returns a pointer to the element at the front of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return TUNDRA_TYPE* Pointer to front element. 
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(TUNDRA_NAME *arr)
{
    return &arr->data[0];
}

/**
 * @brief Returns a const-pointer to the element at the front of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to front element. 
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_cst)(
    const TUNDRA_NAME *arr)
{
    return arr->data;;
}   

/**
 * @brief Returns a pointer to the element at the back of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return TUNDRA_TYPE* Pointer to back element. 
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(back)(TUNDRA_NAME *arr)
{
    return arr->data + TUNDRA_CAPACITY - 1;
}

/**
 * @brief Returns a const-pointer to the element at the back of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return const TUNDRA_TYPE* Const-pointer to back element. 
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(back_cst)(
    const TUNDRA_NAME *arr)
{
    return arr->data + TUNDRA_CAPACITY - 1;
}

/**
 * @brief Returns the fixed size of the Array.
 * 
 * @note Inline method.
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
#undef TUNDRA_INT_FUNC_NAME
