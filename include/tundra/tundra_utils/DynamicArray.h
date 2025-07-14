/**
 * @file DynamicArray.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Expanding container for storing procedurally added elements.
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
#include "tundra/tundra_utils/Memory.h"

#endif // TUNDRA_HGUARD_DYNAMICARRAY_H


// Macros ----------------------------------------------------------------------

// Data type the DynamicArray stores.
#ifndef TUNDRA_DYNARR_TYPE
#define TUNDRA_DYNARR_TYPE_MANUALLY_SET
#define TUNDRA_DYNARR_TYPE int
#endif

// Name identifier for the specific type instance of a DynamicArray.
#ifndef TUNDRA_DYNARR_NAME
#define TUNDRA_DYNARR_NAME_MANUALLY_SET
#define TUNDRA_DYNARR_NAME NO_SPECIFIED_NAME
#endif

// Full signature of the DynamicArray struct.
#define TUNDRA_DYNARR_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_DynamicArray, TUNDRA_DYNARR_NAME)

// Function signature for a DynamicArray of a given type.
#define TUNDRA_DYNARR_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_DynArr, TUNDRA_DYNARR_NAME, func_name)

// Internal function signature for a DynamicArray of a given type.
#define TUNDRA_DYNARR_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(InternalTundra_DynArr, TUNDRA_DYNARR_NAME, func_name)

// Size in bytes of the specified DynamicArray type.
#define TUNDRA_DYNARR_TYPE_SIZE sizeof(TUNDRA_DYNARR_TYPE)

// -----------------------------------------------------------------------------


/**
 * @brief Expanding container for storing procedurally added elements.
 * 
 * Internal variables are read-only.
 */
typedef struct
{
    // Pointer to heap memory containing added elements.
    TUNDRA_DYNARR_TYPE* data;
    
    // Number of elements added to the array.
    uint64_t num_elements;

    // Max number of elements the array can currently hold before needing to 
    // resize.
    uint64_t capacity;

} TUNDRA_DYNARR_STRUCT_SIG;


// Private Methods -------------------------------------------------------------

/**
 * @brief Checks if the array has filled its allocated capacity with elements,
 * and resizes it if it has.
 * 
 * This is an internal function, users should disregard it.
 * 
 * @param array Array to handle.
 */
static inline void TUNDRA_DYNARR_INTFUNC_SIG(_check_and_handle_resize)(
    TUNDRA_DYNARR_STRUCT_SIG *arr)
{
    if(arr->num_elements < arr->capacity) return;

    TUNDRA_DYNARR_TYPE* new_memory = Tundra_alloc_and_copy_memory(arr->data, 
        arr->num_elements * TUNDRA_DYNARR_TYPE_SIZE, 
        arr->capacity * 2 * TUNDRA_DYNARR_TYPE_SIZE);
    
    free(arr->data);
    arr->data = new_memory;
    arr->capacity *= 2;
}


// Public Methods --------------------------------------------------------------

/** 
 * @brief Initializes the array, allocating memory for the initial size and 
 * setting internal components.
 * 
 * If `init_capacity` is set as 0, a default capacity of 2 elements is 
 * allocated. If the desire is to just have an empty created array with no 
 * memory allocated, do not initialize the array until it needs to be used.
 * 
 * @param arr Array to initialize.
 * @param init_capacity Initial capacity in elements to allocate.
*/
static inline void TUNDRA_DYNARR_FUNC_SIG(_init)(
    TUNDRA_DYNARR_STRUCT_SIG *arr, uint64_t init_capacity)
{
    // Set initial capacity to 2 if it is passed as 0.
    init_capacity += 2 * (init_capacity == 0);

    arr->data = (TUNDRA_DYNARR_TYPE*)malloc(
        TUNDRA_DYNARR_TYPE_SIZE * init_capacity);
    arr->capacity = init_capacity;
    arr->num_elements = 0;
}

/**
 * @brief Handles deletion of heap allocated memory for this array.
 * 
 * The array can be safely discarded after this method is called.
 * 
 * @param array Array to deconstruct.
 */
static inline void TUNDRA_DYNARR_FUNC_SIG(_deconstruct)(
    TUNDRA_DYNARR_STRUCT_SIG *arr)
{
    free(arr->data);
    arr->data = NULL;
}

/**
 * @brief Adds an element to the end of the array.
 * 
 * @param array array instance to modify.
 * @param element Element to add.
 */
static inline void TUNDRA_DYNARR_FUNC_SIG(_add)(
    TUNDRA_DYNARR_STRUCT_SIG *arr, const TUNDRA_DYNARR_TYPE *element)
{
    // Reallocate more space in the array if it is full.
    TUNDRA_DYNARR_INTFUNC_SIG(_check_and_handle_resize)(arr);

    // Add the element at the end of the previously added items. 
    arr->data[arr->num_elements++] = *element;
}

/**
 * @brief Ensures the array has the capacity to store `extra_elements`, 
 * resizing and reallocating if necessary.
 * 
 * @param arr Array to reserve.
 * @param extra_elements Number of extra elements to reserve for.
 */
static void TUNDRA_DYNARR_FUNC_SIG(_reserve)(
    TUNDRA_DYNARR_STRUCT_SIG *arr, uint64_t extra_elements)
{
    arr->capacity = Tundra_reserve_memory((void**)(&arr->data), 
        extra_elements * TUNDRA_DYNARR_TYPE_SIZE, 
        arr->num_elements * TUNDRA_DYNARR_TYPE_SIZE, 
        arr->capacity * TUNDRA_DYNARR_TYPE_SIZE);
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
static inline bool TUNDRA_DYNARR_FUNC_SIG(_erase)(
    TUNDRA_DYNARR_STRUCT_SIG *arr, uint64_t index)
{
    // Subtract 1 from num elements here since we don't need to do any shifting
    // if the index to remove is the last valid index. Just decrement the 
    // number of elements to reflect the item is "gone" and it will be
    // overwritten on next `add` call.
    if(index >= arr->num_elements - 1) return false;

    // Copy the elements ahead of the index back one position.
    memmove(arr->data + index, 
        arr->data + index + 1,
        (arr->num_elements - index - 1) * TUNDRA_DYNARR_TYPE_SIZE);

    --arr->num_elements;
    return true;
}

/**
 * @brief Retruns a pointer to the last element in the array of elements.
 * 
 * @attention This method assumes the array is not empty. Calling it with
 * and empty array will result in undefined behavior!
 */
static inline TUNDRA_DYNARR_TYPE* TUNDRA_DYNARR_FUNC_SIG(_back)(TUNDRA_DYNARR_STRUCT_SIG *arr)
{
    return &arr->data[arr->num_elements - 1];
}

/**
 * @brief Returns a pointer to the value at an index.
 * 
 * Performs bounds checking on the index.
 */
static inline TUNDRA_DYNARR_TYPE* TUNDRA_DYNARR_FUNC_SIG(_at)(TUNDRA_DYNARR_STRUCT_SIG *arr, 
    uint64_t index)
{
    // If this is a valid index.
    if(index < arr->num_elements) return arr->data + index;
    
    // else -> Invalid Index.

    return NULL;
}

/**
 * @brief Returns the number of elements that have been added to the array.
 * 
 * @param arr Array to analyze.
 */
static inline uint64_t TUNDRA_DYNARR_FUNC_SIG(_size)(TUNDRA_DYNARR_STRUCT_SIG *arr)
{
    return arr->num_elements;
}

#ifdef TUNDRA_DYNARR_TYPE_MANUALLY_SET
#undef TUNDRA_DYNARR_TYPE_MANUALLY_SET
#undef TUNDRA_DYNARR_TYPE
#endif

#ifdef TUNDRA_DYNARR_NAME_MANUALLY_SET
#undef TUNDRA_DYNARR_NAME_MANUALLY_SET
#undef TUNDRA_DYNARR_NAME
#endif

#undef TUNDRA_DYNARR_STRUCT_SIG
#undef TUNDRA_DYNARR_FUNC_SIG
#undef TUNDRA_DYNARR_INTFUNC_SIG
#undef TUNDRA_DYNARR_TYPE_SIZE
