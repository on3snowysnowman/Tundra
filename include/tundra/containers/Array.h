/**
 * @file Array.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed sized contiguous Array for storing elements.
 * @date 2025-12-05
 * 
 * @copyright Copyright (c) 2025
*/

/**
 * To define an Array type, define the following macros as well as a header 
 * guard before including this file:
 * - TUNDRA_TYPE: Type the Array will store.
 * - TUNDRA_CAP: Capacity of the Array.
 * If these macros are not defined, they will default to:
 * - TUNDRA_TYPE: int
 * - TUNDRA_CAP: 4
*/

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/FatalHandler.h"

#define TUNDRA_ARR_DEF_CAP 4

#ifndef TUNDRA_TYPE
#define TYPE int
#else
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

#ifndef TUNDRA_CAP
#define CAP TUNDRA_EXPAND(TUNDRA_ARR_DEF_CAP)
#else 
#define CAP TUNDRA_EXPAND(TUNDRA_CAP)
#endif

#define NAME TUNDRA_CONCAT3(Tundra_Array, CAP, TUNDRA_EXPAND(TYPE))
#define FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Arr, CAP, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_Arr, CAP, TYPE, _##name)

#define TUNDRA_MAKE_ARR(...) \
    { .data = {__VA_ARGS__} }

#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size contiguous container for storing elements. 
*/
typedef struct NAME
{
    TYPE data[CAP];
} NAME;


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
 * @return TYPE* Pointer to the item at the index. 
 */ 
static inline TYPE* FUNC_NAME(at)(NAME *arr, uint64 index)
{
    if(index >= CAP)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            CAP);
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
 * @return const TYPE* Const-pointer to the item at the index. 
 */
static inline const TYPE* FUNC_NAME(at_cst)(const NAME *arr, 
    uint64 index)
{
    if(index >= CAP)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            CAP);
    }

    return arr->data + index;;
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
 * @return TYPE* Pointer to the item at the index. 
 */
static inline TYPE* FUNC_NAME(at_nochk)(NAME *arr, 
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
 * @return const TYPE* Const-pointer to the item at the index. 
 */
static inline const TYPE* FUNC_NAME(at_nochk_cst)(const NAME *arr, 
    uint64 index)
{
    return arr->data + index;
}

/**
 * @brief Returns a pointer to the element at the front of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return TYPE* Pointer to front element. 
 */
static inline TYPE* FUNC_NAME(front)(NAME *arr)
{
    return &arr->data[0];
}

/**
 * @brief Returns a const-pointer to the element at the front of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return const TYPE* Const-pointer to front element. 
 */
static inline const TYPE* FUNC_NAME(front_cst)(const NAME *arr)
{
    return arr->data;;
}   

/**
 * @brief Returns a pointer to the element at the back of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return TYPE* Pointer to back element. 
 */
static inline TYPE* FUNC_NAME(back)(NAME *arr)
{
    return arr->data + CAP - 1;
}

/**
 * @brief Returns a const-pointer to the element at the back of the Array.
 * 
 * @param arr Array to query.
 * 
 * @return const TYPE* Const-pointer to back element. 
 */
static inline const TYPE* FUNC_NAME(back_cst)(const NAME *arr)
{
    return arr->data + CAP - 1;
}

/**
 * @brief Returns the fixed size of the Array.
 * 
 * @note Inline method.
 *
 * @return uint64 Capacity.
 */
static inline uint64 FUNC_NAME(size)()
{
    return CAP;
}


#ifdef __cplusplus
} // extern "C"
#endif  


#undef TYPE
#undef CAP
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
