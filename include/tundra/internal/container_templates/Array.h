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

// Type and Function Name Macros -----------------------------------------------
#define TUNDRA_NAME TUNDRA_CONCAT3(Tundra_Array, TUNDRA_CAPACITY, \
    TUNDRA_EXPAND(TUNDRA_TYPE))
#define TUNDRA_ITER_NAME TUNDRA_CONCAT3(Tundra_ArrayIterator, TUNDRA_CAPACITY, \
    TUNDRA_TYPENAME)

#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Arr, TUNDRA_CAPACITY, \
    TUNDRA_TYPE, _##name)
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_Arr, \
    TUNDRA_CAPACITY, TUNDRA_TYPE, _##name)
#define TUNDRA_ITER_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_ArrIter, \
    TUNDRA_CAPACITY, TUNDRA_TYPENAME, _##name)


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size contiguous container for storing elements.
 * 
 * Stack allocated array with a fixed capacity. This container requires no
 * initialization or cleanup.
 */
typedef struct TUNDRA_NAME
{
    TUNDRA_TYPE data[TUNDRA_CAPACITY];
} TUNDRA_NAME;

/**
 * @brief Iterator for the Array.
 * 
 */
typedef struct TUNDRA_ITER_NAME
{
    // Pointer to the Array being iterated over.
    TUNDRA_NAME *array;

    // Current index in the Array.
    uint64 index;
} TUNDRA_ITER_NAME;


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
        .index = TUNDRA_CAPACITY
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
#undef TUNDRA_FUNC_NAME
#undef TUNDRA_INT_FUNC_NAME
