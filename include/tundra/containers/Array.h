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

#define TUNDRA_MAKE_ARR(name, ...) \
    (name){ .data = {__VA_ARGS__} }

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

static inline uint64 FUNC_NAME(size)(const NAME *arr)
{
    return CAP;
}


#ifdef __cplusplus
} // extern "C"
#endif  


#undef TYPE
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
