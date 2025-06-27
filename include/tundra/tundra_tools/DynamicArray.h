/**
 * @file Tundra_DynamicArray.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic dynamic size array interface for storing arbitrary types.
 * @version 0.1
 * @date 06-26-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief A generic, dynamically resizing Array for storing elements of 
 * arbitrary types.
 * 
 * Provides fixed-time access and supports procedural addition and automatic 
 * resizing.
 * 
 * Users should NOT modify internal variables, they are read-only.
 */
typedef struct Tundra_DynamicArray 
{
    // Pointer to the data on heap.
    void* data; 

    // Size of the type of data this Array contains.
    uint16_t data_type_size; 

    // Number of elements that have been added to the Array.
    size_t num_elements; 

    // Max number of elements the Array can currently hold before needing to 
    // resize.
    size_t capacity;

} Tundra_DynamicArray;


/**
 * @brief Initializes a DynamicArray and allocates space for at least
 * `starting_capacity` elements.
 *
 * If `starting_capacity` is less than 2, a minimum capacity of 2 is allocated.
 *
 * @param array DynamicArray to initialize.
 * @param data_type_size Size in bytes of each element to be stored.
 * @param starting_capacity Initial number of elements to allocate space for.
 */
void Tundra_DynamicArray_init_array(Tundra_DynamicArray* array, 
    uint32_t data_type_size, size_t starting_capacity);

/**
 * @brief Handles deletion of heap allocated memory for this Array.
 * 
 * Array's data pointer will be set to nullptr, and its values will be zeroed
 * out. The Array can be safely discarded after this method is called.
 * 
 * @param array Array to delete.
 */
void Tundra_DynamicArray_delete_array(Tundra_DynamicArray* array);

/**
 * @brief Adds an element to the end of the Array.
 * 
 * @param array Array instance to modify.
 * @param element Element to add.
 */
void Tundra_DynamicArray_add_element(Tundra_DynamicArray* array, void* element);



/**
 * @brief Gets a pointer to the value at the passed index.
 * 
 * Performs bounds checking on the index.
 * 
 * @param array Array to fetch from.
 * @param index Index into the Array.
 */
void* Tundra_DynamicArray_at(Tundra_DynamicArray* array, size_t index);

/**
 * @brief Removes the element at the specified index and shifts subsequent 
 * elements back by one position.
 *
 * Performs bounds checking on the index.
 *
 * @param array Pointer to the DynamicArray to modify.
 * @param index Index of the element to remove.
 */
void Tundra_DynamicArray_erase_element(Tundra_DynamicArray* array, 
    size_t index);

#endif // DYNAMIC_ARRAY_H