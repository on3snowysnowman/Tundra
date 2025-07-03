/**
 * @file DynamicStack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Expanding container providing LIFO behavior for storing elements.
 * @version 0.1
 * @date 07-02-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_DYNAMICSTACK_H
#define TUNDRA_HGUARD_DYNAMICSTACK_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tundra/tundra_utils/MacroUtils.h"
#include "tundra/tundra_utils/Memory.h"

#endif // TUNDRA_HGUARD_DYNAMICSTACK_H


// Macros ----------------------------------------------------------------------

// Data type the DynamicStack stores.
#ifndef TUNDRA_TYPE
#define TUNDRA_TYPE_MANUALLY_SET
#define TUNDRA_TYPE int
#endif

// Name identifier for the specific type instance of the DynamicStack.
#ifndef TUNDRA_NAME
#define TUNDRA_NAME_MANUALLY_SET
#define TUNDRA_NAME Dflt
#endif

// If the type the FixedStack stores is a primitive type (int, char, float...).
#ifndef TUNDRA_TYPE_IS_PRIMITIVE
#define TUNDRA_PRIMITIVE_MANUALLY_SET
#define TUNDRA_TYPE_IS_PRIMITIVE 0
#endif

// Full signature of the FixedStack struct.
#define TUNDRA_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_DynamicStack, TUNDRA_NAME)

// Function signature for a DynamicStack of a given type.
#define TUNDRA_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_DynStk, TUNDRA_NAME, func_name)

// Internal function signature for a DynamicStack of a given type.
#define TUNDRA_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(InternalTundra_DynStk, TUNDRA_NAME, func_name)

// Size in bytes of the specified DynamicArray type.
#define TUNDRA_TYPE_SIZE sizeof(TUNDRA_TYPE)

// -----------------------------------------------------------------------------

/**
 * @brief Expanding container providing LIFO behavior for storing elements.
 * 
 * Users should NOT modify internal components, they are read-only!
 */
typedef struct
{
    // Array of elements pushed onto the stack.
    TUNDRA_TYPE* data;

    // Number of elements on the stack.
    uint64_t num_elements;

    // Max number of elements the array can currently hold before needing to 
    // resize.
    uint64_t capacity;

} TUNDRA_STRUCT_SIG;


// Private Methods -------------------------------------------------------------

/**
 * @brief Resizes and reallocates the stack to double its capacity.
 * 
 * This is an internal function, users should disregard it.
 */
void TUNDRA_INTFUNC_SIG(_resize)(TUNDRA_STRUCT_SIG *stk)
{
    // Allocate new memory that is double the current capacity and copy the 
    // current elements into the new memory block.
    TUNDRA_TYPE* new_memory = Tundra_alloc_and_copy_memory(stk->data, 
        TUNDRA_TYPE_SIZE * stk->num_elements, stk->capacity * 2);

    free(stk->data);

    stk->data = new_memory;
    stk->capacity *= 2;
}


// Public Methods --------------------------------------------------------------


/** 
 * @brief Initializes the stack, allocating memory for the initial size and 
 * setting internal components.
 * 
 * If `init_capacity` is set as 0, a default capacity of 2 elements is 
 * allocated. If the desire is to just have an empty created stack with no 
 * memory allocated, do not initialize the stack until it needs to be used.
 * 
 * @param stk Stack to initialize.
 * @param init_capacity Initial capacity in elements to allocate.
*/
static inline void TUNDRA_FUNC_SIG(_init)(TUNDRA_STRUCT_SIG *stk, 
    uint64_t init_capacity)
{
    init_capacity += 2 * (init_capacity == 0);

    stk->data = (TUNDRA_TYPE*)malloc(TUNDRA_TYPE_SIZE * init_capacity);
    stk->capacity = init_capacity;
    stk->num_elements = 0;
}

/**
 * @brief Handles deletion of heap allocated memory for this stack.
 * 
 * The stack can be safely discarded after this method is called.
 * 
 * @param array Stack to deconstruct.
 */
static inline void TUNDRA_FUNC_SIG(_deconstruct)(TUNDRA_STRUCT_SIG *stk)
{
    free(stk->data);
    stk->data = NULL;
}

/**
 * @brief "Clears" the stack of added elements.
 * 
 * Internally, no memory is zeroed out or overridden. The variable for tracking
 * the number of elements is simply set to 0 so that when new elements are 
 * pushed, they will overwrite the old data starting at the beginning of the 
 * stack.
 * 
 * @param stack Stack to clear.
 */
static inline void TUNDRA_FUNC_SIG(_clear)(TUNDRA_STRUCT_SIG *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Pops an element off the stack.
 * 
 * This method does not return the popped element. Call the `back` method to get 
 * a reference to the last element.
 */
static inline void TUNDRA_FUNC_SIG(_pop)
    (TUNDRA_STRUCT_SIG *stk)
{
    --stk->num_elements;
}

/**
 * @brief Reduces the stack's capacity to a specified value.
 *
 * If `new_capacity` is less than the current number of elements, excess 
 * elements will be discarded. If `new_capacity` is greater than or equal to the 
 * current capacity, no action is taken. The stack's memory will be reduced and 
 * reallocated to fit the new capacity.
 *
 * @param stk Stack to shrink.
 * @param new_capacity The desired new capacity.
 */
static void TUNDRA_FUNC_SIG(_shrink)(TUNDRA_STRUCT_SIG *stk,
    uint64_t new_capacity)
{
    if(new_capacity >= stk->capacity) return;

    // If the new capacity is less than the current pushed elements, we only 
    // want to copy the elements within the range of the new capacity. 
    // Otherwise, the new capacity is greater than or equal to the current 
    // pushed elements, and we can just copy all the elements.

    uint64_t num_bytes_to_copy;

    if(new_capacity < stk->num_elements) 
    {
        num_bytes_to_copy = new_capacity * TUNDRA_TYPE_SIZE;
        
        // The stack is being shrunk below the current number of elements,
        // `num_elements` needs to be updated to this smaller value.
        stk->num_elements = new_capacity;
    }

    // Since no elements were pruned, there is no need to update `num_elements`.
    else num_bytes_to_copy = stk->num_elements * TUNDRA_TYPE_SIZE;

    // Allocate a new block and copy the correct number of bytes to it.
    TUNDRA_TYPE *new_memory = Tundra_alloc_and_copy_memory(stk->data,
        num_bytes_to_copy, new_capacity * TUNDRA_TYPE_SIZE);

    free(stk->data);

    stk->data = new_memory;
    stk->capacity = new_capacity;
}

/**
 * @brief Pushes an element onto the stack.
 * 
 * @param stk Stack to modify.
 * @param element Element to push.
 */
static inline bool TUNDRA_FUNC_SIG(_push)(TUNDRA_STRUCT_SIG *stk,
    const TUNDRA_TYPE* element)
{
    // If the stack is full, resize.
    if(stk->num_elements == stk->capacity) TUNDRA_INTFUNC_SIG(_resize)(stk);

    stk->data[stk->num_elements++] = *element;
}

/**
 * @brief Ensures the stack has the capacity to store `extra_elements`, 
 * resizing and reallocating if necessary.
 * 
 * @param stk Stack to reserve.
 * @param extra_elements Number of extra elements to reserve for.
 */
static void TUNDRA_FUNC_SIG(_reserve)(
    TUNDRA_STRUCT_SIG *stk, uint64_t extra_elements)
{
    stk->capacity = Tundra_reserve_memory((void**)(&stk->data), 
        extra_elements * TUNDRA_TYPE_SIZE, stk->num_elements * TUNDRA_TYPE_SIZE, 
        stk->capacity * TUNDRA_TYPE_SIZE);
}

/**
 * @brief Returns true if the stack is empty.
 */
static inline bool TUNDRA_FUNC_SIG(_is_empty)(TUNDRA_STRUCT_SIG *stk)
{
    return stk->num_elements == 0;
}

/**
 * @brief Returns the the number of elements on the stack.
 * 
 * @param stk Stack to analyze.
 */
static inline uint64_t TUNDRA_FUNC_SIG(_size)(TUNDRA_STRUCT_SIG *stk)
{
    return stk->num_elements;
}

/**
 * @brief Returns the current capacity of the stack.
 */
static inline uint64_t TUNDRA_FUNC_SIG(_capacity)(TUNDRA_STRUCT_SIG *stk)
{
    return stk->capacity;
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
