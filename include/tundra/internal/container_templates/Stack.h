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
#endif

#define TUNDRA_NAME TUNDRA_CONCAT3(Tundra_Stack, TUNDRA_CAPACITY, \
    TUNDRA_EXPAND(TUNDRA_TYPE))
#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_Stk, TUNDRA_CAPACITY, \
    TUNDRA_TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
<<<<<<< HEAD
 * Requires initialization using the `init` method, but does not need to be 
 * freed with a `free` call.
=======
 * Stack allocated stack with a fixed capacity. This container requires no 
 * cleanup as all memory is managed on the stack. 
 * 
>>>>>>> 1106b43deafcd2abf9d95ff370a13936feecdb1f
 */
typedef struct TUNDRA_NAME
{
    // Stack allocated array of elements. The index "`num_elem` - 1" into this 
    // array represents the "top" element on the stack, which is what is 
    // returned by the `front` method, and is the first to be popped off with 
    // `pop`.
    TUNDRA_TYPE data[TUNDRA_CAPACITY];

    // Number of elements currently on the Stack. Cannot exceed fixed capacity.
    uint64 num_elem;
} TUNDRA_NAME;


// Public Methods --------------------------------------------------------------

<<<<<<< HEAD
/**
 * @brief Initializes a Stack, setting internal components.
 * 
 * Only initialize a Stack once. If an already initialized Stack is called with 
 * init, undefined behavior may occur.
 * 
 * @param stk Stack to init.
 */
static inline void TUNDRA_FUNC_NAME(init)(TUNDRA_NAME *stk)
{
    stk->num_elem = 0;
}

/**
 * @brief Clears the Stack of elements.
 * 
 * @param stk Stack to clear.
 */
static inline void TUNDRA_FUNC_NAME(clear)(TUNDRA_NAME *stk)
{
    stk->num_elem = 0;
}

/**
 * @brief Pushes a copy of an element onto the Stack, returning true if there 
 * was space. If the Stack is full, returns false and does not push the element.
 * 
 * @param stk Stack to push to.
 * @param element Pointer to the element to push.
 * 
 * @return bool True if the element was successfully pushed, false if the Stack
 * was full.
 */
static inline bool TUNDRA_FUNC_NAME(push)(TUNDRA_NAME *stk, 
    const TUNDRA_TYPE *element)
{
    if(stk->num_elem >= TUNDRA_CAPACITY) { return false; }

    stk->data[stk->num_elem] = *element;
    ++stk->num_elem;
    return true;
}

/**
 * @brief Pops a value off the top of the Stack.
 * 
 * A fatal is thrown if the Stack is empty with the Stack unchanged.
 * 
 * @param stk Stack to pop.
 */
static inline void TUNDRA_FUNC_NAME(pop)(TUNDRA_NAME *stk)
{
    if(stk->num_elem == 0)
    {
        TUNDRA_FATAL("Atempted to pop but the Stack was empty.");
        return;
    }

    --stk->num_elem;
}

/**
 * @brief Returns true if the Stack is empty.
 * 
 * @param stk Stack to query.
 * 
 * @return bool True if the Stack contains no elements.
 */
static inline bool TUNDRA_FUNC_NAME(is_empty)(const TUNDRA_NAME *stk)
{
    return stk->num_elem == 0;
}

/**
 * @brief Returns true if the Stack has reached its capacity. 
 * 
 * @param stk Stack to query.
 * 
 * @return bool True if the Stack has no more room for elements.
 */
static inline bool TUNDRA_FUNC_NAME(is_full)(const TUNDRA_NAME *stk)
{
    return stk->num_elem == TUNDRA_CAPACITY;
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
 * @return TUNDRA_TYPE* Pointer to the top element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(TUNDRA_NAME *stk)
{
    return stk->data + stk->num_elem - 1;
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
 * @return const TUNDRA_TYPE* Const-pointer to the top element.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_cst)(
    const TUNDRA_NAME *stk)
{
    return stk->data + stk->num_elem - 1;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Stack to query.
 * 
 * @return uint64 Number of elements on the Stack.
 */
static inline uint64 TUNDRA_FUNC_NAME(size)(const TUNDRA_NAME *stk)
{
    return stk->num_elem;
}

/**
 * @brief Returns the fixed capacity of the Stack type. 
 * 
 */
static inline uint64 TUNDRA_FUNC_NAME(capacity)()
{
    return TUNDRA_CAPACITY;
}


#ifdef __cplusplus
} // extern "C"
#endif  

#undef TUNDRA_NAME
#undef TUNDRA_FUNC_NAME
=======

>>>>>>> 1106b43deafcd2abf9d95ff370a13936feecdb1f
