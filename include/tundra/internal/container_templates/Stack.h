/**
 * @file Stack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_STACK_H
#define TUNDRA_STACK_H
#define TUNDRA_STK_DEF_CAP 4
#define TUNDRA_MAKE_STACK(...) \
    { .data = {__VA_ARGS__} }
#endif

#define TUNDRA_NAME TUNDRA_CONCAT3(Tundra_Stack, TUNDRA_CAPACITY, \
    TUNDRA_EXPAND(TUNDRA_TYPE))
#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Stk, TUNDRA_CAPACITY, \
    TUNDRA_TYPE, _##name)
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_Stk, \
    TUNDRA_CAPACITY, TUNDRA_TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
 * Stack allocated stack with a fixed capacity. This container requires no 
 * initialization or cleanup as all memory is managed on the stack.
 * 
 */
typedef struct TUNDRA_NAME
{
    TUNDRA_TYPE data[TUNDRA_CAPACITY]
} TUNDRA_NAME;


// Public Methods --------------------------------------------------------------