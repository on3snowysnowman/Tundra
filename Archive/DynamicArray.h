/**
 * @file Tundra_DynamicArray.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic dynamic size array for storing arbitrary types with automatic
 *        resizing.
 * @version 0.1
 * @date 06-26-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_DYNAMIC_ARRAY_H
#define TUNDRA_HGUARD_DYNAMIC_ARRAY_H

#include <stdint.h>                                                             
#include <stddef.h>                                                             
#include <stdbool.h>                                                            
#include <stdlib.h>                                                             
#include <string.h>      
#include <stdio.h>                                                       

#ifndef Tundra_ArrType
#define Tundra_ArrType int
#endif

#define CONCAT(a, b) a##b

typedef Tundra_ArrType Arr_Type;

/**
 * @brief A generic, dynamically resizing Array for storing elements of 
 * arbitrary types.
 * 
 * Provides fixed-time access and supports procedural addition and automatic 
 * resizing.
 * 
 * Users should NOT modify internal variables, they are read-only.
 */
typedef struct
{
    // Pointer to the data on heap.
    void *data; 

    // Size of the data type stored.
    uint16_t data_type_size; 

    // Number of elements that have been added to the Array.
    uint64_t num_elements; 

    // Max number of elements the Array can currently hold before needing to 
    // resize.
    uint64_t capacity;

} Tundra_DynamicArray;


/**
 * @brief Initializes the Array, allocating enough memory for 
 * `init_capacity` elements and setting internal components. 
 * 
 * If `init_capacity` is less than 2, a minimum capacity of 2 bytes is 
 * allocated. If the desire is to just have an emtpy created Array with no 
 * memory allocated, do not initialize the Array. Just be sure to call this 
 * method before performing any operations on it.
 *
 * @param array DynamicArray to initialize.
 * @param data_type_size Size in bytes of each element to be stored.
 * @param init_capacity Initial number of elements to allocate space for.
 */
void Tundra_DynArr_init(Tundra_DynamicArray *array, 
    uint32_t data_type_size, uint64_t init_capacity);

/**
 * @brief Handles deletion of heap allocated memory for this Array.
 * 
 * The Array can be safely discarded after this method is called.
 * 
 * @param array Array to delete.
 */
void Tundra_DynArr_deconstruct(Tundra_DynamicArray *array);

/**
 * @brief Adds an element(lvalue) to the end of the Array.
 * 
 * If you want to add an rvalue to the Array, use the macro 
 * `TUNDRA_DYNARR_ADD_RVAL`.
 * 
 * The bytes of the pointed to element are copied into the Array.
 * 
 * @param array Array instance to modify.
 * @param element Element to add.
 */
void Tundra_DynArr_add_element(Tundra_DynamicArray *array, 
    const void* element);

/**
 * @brief Removes the element at the specified index and shifts subsequent 
 * elements back by one position.
 *
 * Performs bounds checking on the index.
 *
 * @param array Pointer to the DynamicArray to modify.
 * @param index Index of the element to remove.
 */
void Tundra_DynArr_erase_element(Tundra_DynamicArray *array, 
    uint64_t index);

/**
 * @brief Ensures the Array has enough capacity to store at least 
 * `extra_elements` more elements beyond its current number of added elements.
 * 
 * If the current capacity is sufficient, no action is taken. Otherwise, 
 * the internal storage is reallocated to accommodate at least 
 * (`capacity` + `extra_elements`) elements.
 * 
 * @param array DynamicArray to modify.
 * @param extra_elements Number of additional elements to reserve space for.
 */
void Tundra_DynArr_reserve(Tundra_DynamicArray *array, 
    uint64_t extra_elements);

/**
 * @brief Gets a pointer to the value at the passed index.
 * 
 * Performs bounds checking on the index.
 * 
 * @param array Array to fetch from.
 * @param index Index into the Array.
 */
void* Tundra_DynArr_at(Tundra_DynamicArray *array, uint64_t index);

#endif // TUNDRA_HGUARD_DYNAMIC_ARRAY_H
