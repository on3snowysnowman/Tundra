/**
 * @file Stack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * @date 2025-12-05
 * 
 * @copyright Copyright (c) 2025
 */

/**
 * To define a Stack type, define the following macros as well as a header guard
 * before including this file:
 * - TUNDRA_TYPE: Type the Stack will store.
 * - TUNDRA_CAP: Capacity of the Stack.
 * If these macros are not defined, they will default to:
 * - TUNDRA_TYPE: int
 * - TUNDRA_CAP: 4
*/

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_STACK_H
#define TUNDRA_STACK_H
#define TUNDRA_STK_DEF_CAP 4
#endif

#ifndef TUNDRA_TYPE
#define TYPE int
#else
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

#ifndef TUNDRA_CAP
#define CAP TUNDRA_EXPAND(TUNDRA_STK_DEF_CAP)
#else 
#define CAP TUNDRA_EXPAND(TUNDRA_CAP)
#endif

#define NAME TUNDRA_CONCAT3(Tundra_Stack, CAP, TUNDRA_EXPAND(TYPE))
#define FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Stk, CAP, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_Stk, CAP, TYPE, _##name)

#define TUNDRA_MAKE_STK(type, ...) \
    { .data = {__VA_ARGS__}, \
    .num_elements = sizeof((type[]){__VA_ARGS__}) / sizeof(type) }


#ifdef __cplusplus
extern "C" {
#endif

// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
*/
typedef struct NAME
{
    // Fixed array that stores the elements pushed on the Stack.
    TYPE data[CAP];
    
    // Number of elements on the Stack.
    uint64 num_elements;
} NAME;


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes the Stack, resetting internal components.
 * 
 * @param stk Stack to init.
 */
static inline void FUNC_NAME(init)(NAME *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Resets the Stack to an empty state.
 * 
 * This does not modify or zero out the underlying memory. Only the element 
 * count is reset to zero, so subsequent pushes will overwrite previous data 
 * from the start of the Stack.
 * 
 * @param stk Stack to clear.
 */
static inline void FUNC_NAME(clear)(NAME *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Attempts to push an element onto the Stack.
 *
 * If the Stack is full, the push fails and returns false.
 * Otherwise, the element is added to the top of the Stack and returns true.
 *
 * @param stk Stack to push to.
 * @param element Pointer to the element to push.
 * 
 * @return True if the element was successfully pushed, false if the Stack is 
 * full.
*/
static inline bool FUNC_NAME(push)(NAME *stk, TYPE element)
{
    if(stk->num_elements >= CAP) { return false; }

    stk->data[stk->num_elements++] = element;
    return true;
}

/**
 * @brief Pops a value off the top of the Stack.
 * 
 * Performs a size check to ensure the Stack contains elements before popping.
 *
 * Does not return the popped value. Call the `back` method to get a reference
 * to the last element before popping.
 * 
 * @param stk Stack to pop.
 */
static inline void FUNC_NAME(pop)(NAME *stk)
{
    if(stk->num_elements > 0) 
    {
        --stk->num_elements;
        return;
    }

    TUNDRA_FATAL("Attempted to pop but Stack was empty.");
}

/**
 * @brief Checks whether the Stack contains any elements, returning true if
 * it is empty.
 *
 * @param stk Stack to check.
 * 
 * @return bool True if the Stack has no elements, false otherwise.
*/
static inline bool FUNC_NAME(is_empty)(const NAME *stk)
{
    return !stk->num_elements;
}

/**
 * @brief Checks if the Stack is full, returning true if it is.
 * 
 * @param stk Stack to check.
 * 
 * @return bool True if the Stack is full, false otherwise.
 */
static inline bool FUNC_NAME(is_full)(const NAME *stk)
{
    return stk->num_elements >= CAP;
}

/**
 * @brief Returns a pointer to the element at the top of the Stack.
 * 
 * @attention For fast access, this method does not perform a check if the 
 * Stack is empty. It is the user's responsibility to ensure the Stack is not 
 * empty.
 * 
 * @param stk Stack to query.
 * 
 * @return TYPE* Pointer to the top element.
 */
static inline TYPE* FUNC_NAME(front)(NAME *stk)
{
    return stk->data;
}

/**
 * @brief Returns a const-pointer to the element at the top of the Stack.
 * 
 * @attention For fast access, this method does not perform a check if the 
 * Stack is empty. It is the user's responsibility to ensure the Stack is not 
 * empty.
 * 
 * @param stk Stack to query.
 * 
 * @return const TYPE* Const-pointer to the top element.
 */
static inline const TYPE* FUNC_NAME(front_cst)(const NAME *stk)
{
    return stk->data;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Stack to query.
 * 
 * @return [Tundra::uint64] Number of elements on the Stack.
 */
static inline uint64 FUNC_NAME(size)(const NAME *stk)
{
    return stk->num_elements;
}

/**
 * @brief Returns the fixed capacity of the Stack.
 * 
 * @param stk Stack to query.
 *
 * @return uint64 Fixed capacity. 
 */
static inline uint64 FUNC_NAME(capacity)(const NAME *stk)
{
    return CAP;
}


#ifdef __cplusplus
}
#endif


#undef TYPE
#undef CAP
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
