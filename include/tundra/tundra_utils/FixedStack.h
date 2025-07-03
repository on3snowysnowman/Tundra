/**
 * @file FixedStack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * @version 0.1
 * @date 06-29-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_FIXEDSTACK_H
#define TUNDRA_HGUARD_FIXEDSTACK_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tundra/tundra_utils/MacroUtils.h"

#endif // TUNDRA_HGUARD_FIXEDSTACK_H


// Macros ----------------------------------------------------------------------

// Data type the FixedStack stores.
#ifndef TUNDRA_TYPE
#define TUNDRA_TYPE_MANUALLY_SET
#define TUNDRA_TYPE int
#endif

// Name identifier for the specific type instance of a FixedStack.
#ifndef TUNDRA_NAME
#define TUNDRA_NAME_MANUALLY_SET
#define TUNDRA_NAME Dflt
#endif

// Size of elements to allocate for the FixedStack
#ifndef TUNDRA_CAPACITY
#define TUNDRA_SIZE_MANUALLY_SET
#define TUNDRA_CAPACITY 2
#endif

// Full signature of the FixedStack struct.
#define TUNDRA_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(\
        Tundra_FixedStack,\
        TUNDRA_JOIN_TWO_MACROS(TUNDRA_NAME, TUNDRA_CAPACITY))

// Function signature for a FixedStack of a given type.
#define TUNDRA_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(\
        Tundra_FxdStk,\
        TUNDRA_JOIN_TWO_MACROS(TUNDRA_NAME, TUNDRA_CAPACITY),\
        func_name)

// -----------------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
 * Users should NOT modify internal components, they are read-only!
 */
typedef struct
{
    // Array of elements pushed onto the stack.
    TUNDRA_TYPE data[TUNDRA_CAPACITY];

    // Number of element on the stack.
    uint64_t num_elements;

} TUNDRA_STRUCT_SIG;


// Public Methods --------------------------------------------------------------

/**
 * @brief "Clears" the stack of added elements.
 * 
 * Internally, no memory is zeroed out or overridden. The variable for tracking
 * the number of elements is simply set to 0 so that when new elements are 
 * pushed, they will overwrite the old data starting at the beginning of the 
 * Stack.
 * 
 * @param stack Stack to clear.
 */
static inline void TUNDRA_FUNC_SIG(_clear)(TUNDRA_STRUCT_SIG *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Pops a value off the stack.
 * 
 * This method does not return the popped value. Call the `back` method to get 
 * a reference to the last element.
 */
static inline void TUNDRA_FUNC_SIG(_pop)
    (TUNDRA_STRUCT_SIG *stk)
{
    --stk->num_elements;
}


/**
 * @brief Pushes an element onto the stack if there is space remaining, 
 * returning true if successful.
 * 
 * Performs a check if the stack is full, returns false if it is.
 * 
 * @param stk Stack to modify.
 * @param element Element to push.
 */
static inline bool TUNDRA_FUNC_SIG(_push)(TUNDRA_STRUCT_SIG *stk,
    const TUNDRA_TYPE* element)
{
    if(stk->num_elements == TUNDRA_CAPACITY) return false;

    stk->data[stk->num_elements++] = *element;
    return true;
}

/**
 * @brief Returns true if the stack is empty.
 */
static inline bool TUNDRA_FUNC_SIG(_is_empty)(TUNDRA_STRUCT_SIG *stk)
{
    return stk->num_elements == 0;
}

/**
 * @brief Returns the capacity of the stack.
 */
static inline uint64_t TUNDRA_FUNC_SIG(_capacity)()
{
    return TUNDRA_CAPACITY;
}

/**
 * @brief Returns a pointer to the top element (last added) on the stack.
 * 
 * @attention This method assumes the stack is not empty. Calling it with
 * and empty stack will result in undefined behavior!
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_SIG(_back)(TUNDRA_STRUCT_SIG *stk)
{
    return &stk->data[stk->num_elements - 1];
}

// Cleanup Macro definitions.

#ifdef TUNDRA_TYPE_MANUALLY_SET
#undef TUNDRA_TYPE
#endif

#ifdef TUNDRA_NAME_MANUALLY_SET
#undef TUNDRA_NAME
#endif

#ifdef TUNDRA_SIZE_MANUALLY_SET
#undef TUNDRA_CAPACITY
#endif 

#undef TUNDRA_STRUCT_SIG
#undef TUNDRA_FUNC_SIG

