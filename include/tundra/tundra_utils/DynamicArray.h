/**
 * @file DynamicArray.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Expandable container for storing procedurally added elements.
 * @version 0.1
 * @date 06-28-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_DYNAMICARRAY_H
#define TUNDRA_HGUARD_DYNAMICARRAY_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tundra/tundra_utils/MacroUtils.h"

#endif // TUNDRA_HGUARD_DYNAMICARRAY_H


// Macros ----------------------------------------------------------------------

// Data type the DynamicArray stores.
#ifndef TUNDRA_TYPE
#define TUNDRA_TYPE_MANUALLY_SET
#define TUNDRA_TYPE int
#endif

// Name identifier for the specific type instance of a DynamicArray.
#ifndef TUNDRA_NAME
#define TUNDRA_NAME_MANUALLY_SET
#define TUNDRA_NAME Dflt
#endif

// Full signature of the DynamicArray struct .
#define TUNDRA_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_DynamicArray, TUNDRA_NAME)

// Function signature for a DynamicArray of a given type.
#define TUNDRA_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_DynArr, TUNDRA_NAME, func_name)

// Internal function signature for a DynamicArray of a given type.
#define TUNDRA_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_INTFUNC_SIG(InternalTundra_DynArr, TUNDRA_NAME, \
        func_name)

// Size in bytes of the specified DynamicArray type.
#define TUNDRA_TYPE_SIZE sizeof(TUNDRA_TYPE)

// -----------------------------------------------------------------------------


/**
 * @brief Expandable container for storing procedurally added elements.
 */
typedef struct
{
    // Pointer to heap memory containing added elements.
    TUNDRA_TYPE* data;
    
    // Number of elements added to the array.
    uint64_t num_elements;

    // Max number of elements the array can currently hold before needing to 
    // resize.
    uint64_t capacity;

} TUNDRA_STRUCT_SIG;


// Private Methods -------------------------------------------------------------

/**
 * @brief Resizes and reallocates the array to a new capacity.
 * 
 * This is an internal function, users should NOT call it!
 */
static inline void TUNDRA_INTFUNC_SIG(_resize)(
    TUNDRA_STRUCT_SIG *arr, uint64_t new_capacity)
{
    // This method assumes that previous memory for the array has been allocated
    // and the 'data' ptr is not NULL. 

    // Allocate new block of memory with the new capacity.
    TUNDRA_TYPE* new_memory = malloc(TUNDRA_TYPE_SIZE
        * new_capacity);
    
    // Copy elements into the newly allocated block.
    memcpy(new_memory, arr->data, arr->capacity * TUNDRA_TYPE_SIZE);

    // Free old data.
    free(arr->data);

    arr->data = new_memory;
    arr->capacity = new_capacity;
}

/**
 * @brief Checks if the Array has filled its allocated capacity with elements,
 * and resizes it if it has.
 * 
 * This is an internal function, users should NOT call it!
 * 
 * @param array Array to handle.
 */
static inline void TUNDRA_INTFUNC_SIG(_check_and_handle_resize)(
    TUNDRA_STRUCT_SIG *arr)
{
    if(arr->num_elements == arr->capacity) 
        TUNDRA_INTFUNC_SIG(_resize)(arr, arr->capacity * 2);
}


// Public Methods --------------------------------------------------------------

static inline void TUNDRA_FUNC_SIG(_init)(
    TUNDRA_STRUCT_SIG *arr, uint64_t init_capacity)
{
    // Set initial capacity to 2 if it is passed as 0.
    init_capacity += 2 * (init_capacity == 0);

    arr->data = (TUNDRA_TYPE*)malloc(
        TUNDRA_TYPE_SIZE * init_capacity);
    arr->capacity = init_capacity;
    arr->num_elements = 0;
}

/**
 * @brief Handles deletion of heap allocated memory for this Array.
 * 
 * The Array can be safely discarded after this method is called.
 * 
 * @param array Array to deconstruct.
 */
static inline void TUNDRA_FUNC_SIG(_deconstruct)(
    TUNDRA_STRUCT_SIG *arr)
{
    free(arr->data);
    arr->data = NULL;
}

/**
 * @brief Adds an element to the end of the Array.
 * 
 * @param array Array instance to modify.
 * @param element Element to add.
 */
static inline void TUNDRA_FUNC_SIG(_add)(
    TUNDRA_STRUCT_SIG *arr, const TUNDRA_TYPE element)
{
    // Reallocate more space in the array if it is full.
    TUNDRA_INTFUNC_SIG(_check_and_handle_resize)(arr);

    // Add the element at the end of the previously added items. 
    arr->data[arr->num_elements++] = element;

    // // Set the value at the current index in the array to the passed element.
    // *(arr->data + arr->num_elements) = element; 

    // ++arr->num_elements;
}

static void TUNDRA_FUNC_SIG(_reserve)(
    TUNDRA_STRUCT_SIG *arr, uint64_t extra_elements)
{
    uint64_t remaining_space = arr->capacity - arr->num_elements;

    // If the capacity is already sufficient.
    if(extra_elements <= remaining_space) return;
    // The extra elements is greater than the remaining space plus 
    // another entire capacity, so we can't simply just double the Array's 
    // capacity since it's too small. Allocate new space that is at least enough
    // for the old elements combined with the extra elements.
    else if(extra_elements >= arr->capacity + remaining_space)
        TUNDRA_INTFUNC_SIG(_resize)(arr, arr->capacity + 
            extra_elements);

    // Doubling the capacity is enough.
    else 
        TUNDRA_INTFUNC_SIG(_resize)(arr, arr->capacity * 2);
}

/**
 * @brief Removes the element at the specified index and shifts subsequent 
 * elements back by one position, returns true if successful.
 *
 * Performs bounds checking on the index.
 *
 * @param array Pointer to the DynamicArray to modify.
 * @param index Index of the element to remove.
 */
static inline bool TUNDRA_FUNC_SIG(_erase)(
    TUNDRA_STRUCT_SIG *arr, uint64_t index)
{
    if(index >= arr->num_elements) return false;

    // Copy the elements ahead of the index back one position.
    memmove(arr->data + index, 
        arr->data + index + 1,
        arr->num_elements - index - 1);

    --arr->num_elements;
    return true;
}

/**
 * @brief Retruns a pointer to the last element in the array of elements.
 * 
 * @attention This method assumes the array is not empty. Calling it with
 * and empty array will result in undefined behavior!
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_SIG(_back)(TUNDRA_STRUCT_SIG *arr)
{
    return &arr->data[arr->num_elements - 1];
}

/**
 * @brief Returns a pointer to the value at an index.
 * 
 * Performs bounds checking on the index.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_SIG(_at)(TUNDRA_STRUCT_SIG *arr, 
    uint64_t index)
{
    // If this is a valid index.
    if(index < arr->num_elements) return arr->data + index;
    
    // else -> Invalid Index.

    return NULL;
    
}

#ifdef TUNDRA_TYPE_MANUALLY_SET
#undef TUNDRA_TYPE
#endif

#ifdef TUNDRA_NAME_MANUALLY_SET
#undef TUNDRA_NAME
#endif

#undef TUNDRA_STRUCT_SIG
#undef TUNDRA_FUNC_SIG
#undef TUNDRA_INTFUNC_SIG
