/**
 * @file DynamicArray.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-26-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/tundra_tools/DynamicArray.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// Private Methods ------------------------------------------------------------

/**
 * @brief Returns true if the passed `index` is within the Array's currently
 * added elements count. 
 * 
 * @param array Array to analyze.
 * @param index Index to check.
 */
static bool index_within_elements(const Tundra_DynamicArray* array, size_t index)
{
    return index < array->num_elements;
}

/**
 * @brief Resizes and reallocates the Array's data with a new capacity.
 * 
 * @param array Array to resize.
 * @param new_capacity New capacity to allocate for.
 */
static void resize_array(Tundra_DynamicArray* array, uint64_t new_capacity)
{
    // Allocate new block of memory that is twice the current capacity.
    void* new_memory = malloc(array->data_type_size * new_capacity);

    // If memory has been allocated for previous data.
    if(array->data)
    {
        // Copy old elements into the new memory.
        memcpy(new_memory, array->data, 
            array->capacity * array->data_type_size);

        // Free old data.
        free(array->data);
    }

    // Update the array's capacity.
    array->capacity = new_capacity;

    // Update the data pointer to the new memory.
    array->data = new_memory;
}   



/**
 * @brief Checks if the Array has filled its allocated capacity with elements,
 * and resizes it if it has.
 * 
 * @param array Array to handle.
 */
static void check_and_handle_resize(Tundra_DynamicArray* array)
{
    if(array->num_elements == array->capacity) 
        // Increment the new capacity by 2 if the current capacity is 0. This
        // handles the case when the capacity is initially 0 and is attempted to
        // be doubled here.
        resize_array(array, (array->capacity * 2) + (array->capacity == 0) * 2);
}


// Public Methods -------------------------------------------------------------

void Tundra_DynamicArray_init(Tundra_DynamicArray* array, 
    uint32_t data_type_size, size_t starting_capacity)
{
    if(starting_capacity == 0) array->data = NULL;
    else array->data = malloc(data_type_size * starting_capacity);

    array->num_elements = 0;
    array->capacity = starting_capacity;
    array->data_type_size = data_type_size;
}

void Tundra_DynamicArray_delete_array(Tundra_DynamicArray* array)
{
    free(array->data);
    array->data = NULL;
    array->capacity = 0;
    array->data_type_size = 0;
    array->num_elements = 0;
}

void Tundra_DynamicArray_add_element(Tundra_DynamicArray* array, 
    const void* element)
{
    check_and_handle_resize(array);
    
    // The byte position inside the array where the bytes of the element to add
    // will be copied.
    char* byte_copy_position = 
        (char*)array->data + (array->num_elements * array->data_type_size);

    memcpy(byte_copy_position, element, array->data_type_size);

    ++array->num_elements;
}

void Tundra_DynamicArray_erase_element(Tundra_DynamicArray* array, 
    size_t index)
{
    // If the index is invalid.
    if(!index_within_elements(array, index)) return;

    // Cast to char* for byte pointer manipulation.
    char* casted_data = (char*)array->data;

    // Copy the elements ahead of the index back one position.
    memmove(casted_data + (index * array->data_type_size), 
        casted_data + ((index + 1) * array->data_type_size),
        (array->num_elements - index - 1) * array->data_type_size);

    --array->num_elements;
}

void Tundra_DynamicArray_reserve(Tundra_DynamicArray* array, 
    uint64_t extra_elements)
{
    // Allocated space for elements that has not been filled yet.
    uint64_t remaining_space = array->capacity - array->num_elements;

    // If the capacity is already sufficient.
    if(extra_elements <= remaining_space) return;
    // The extra elements is greater than the remaining space plus 
    // another entire capacity, so we can't simply just double the Array's 
    // capacity since it's too small. Allocate new space that is at least enough
    // for the new elements.
    else if(extra_elements >= array->capacity + remaining_space)
        resize_array(array, extra_elements);

    // Doubling the capacity is enough.
    else 
        resize_array(array, array->capacity * 2);
}

void* Tundra_DynamicArray_at(Tundra_DynamicArray* array, size_t index)
{
    // If the index is invalid. 
    if(!index_within_elements(array, index))
    {
        return NULL;
    }

    return (char*)array->data + (index * array->data_type_size);
}
