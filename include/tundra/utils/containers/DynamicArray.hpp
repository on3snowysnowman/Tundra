/**
 * @file DynamicArray.hpp
 * @author your name (you@domain.com)
 * @brief Automatic resizing container for storing procedurally added elements.
 * @version 0.1
 * @date 2025-07-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "tundra/utils/Memory.hpp"

namespace Tundra::DynArr
{

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for storing procedurally added elements.
 * 
 * The array must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `deconstruct` method when the array is no longer required.
 * 
 * Internals are read-only.
 * 
 * @tparam T Data Type the array stores.
 */
template<typename T>
struct DynamicArray
{
    T* data;

    uint64_t num_elements;
    uint64_t capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{
    
constexpr uint64_t DEFAULT_CAPACITY = 2;

/**
 * @brief Underlying initializes method for the array. Allocates initial memory
 * for `init_capacity` elements and sets internal values.
 * 
 * @tparam T 
 * @param arr Pointer to the array.
 * @param init_capacity Initial capacity in elements to allocate.
 */
template<typename T>
void underlying_init(Tundra::DynArr::DynamicArray<T> *arr, 
    uint64_t init_capacity)
{ 
    arr->data = (T*)malloc(init_capacity * sizeof(T));
    arr->num_elements = 0;
    arr->capacity = init_capacity;
}

/**
 * @brief Checks if the array has filled its allocated capacity, resizing if it
 * has.
 *  
 * @param arr Pointer to the array.
 */
template<typename T>
void check_and_handle_resize(Tundra::DynArr::DynamicArray<T> *arr)
{
    if(arr->num_elements < arr->capacity) return;

    // Get a new memory block that is twice the capacity of the current one.
    T *new_memory = (T*)Tundra::alloc_and_copy_memory(arr->data, 
        arr->num_elements * sizeof(T),
        arr->capacity * 2 * sizeof(T));

    free(arr->data);
    arr->data = new_memory;
    arr->capacity *= 2;
}

/**
 * @brief Underlying shrink method, shrinks the array's capacity to match the 
 * specified new capacity.
 *  
 * @param arr Pointer to the array.
 * @param capacity Capacity to shrink to.
 */
template<typename T>
void underlying_shrink(Tundra::DynArr::DynamicArray<T> *arr, uint64_t capacity)
{
    uint64_t new_capacity_bytes = capacity * sizeof(T);
    
    T* new_memory = Tundra::alloc_and_copy_memory(arr->data, new_capacity_bytes, 
        new_capacity_bytes);

    free(arr->data);
    arr->data = new_memory;
}

} // namespace Internal


// Public ----------------------------------------------------------------------

/**
 * @brief Initializes an array with default capacity. Allocates memory and 
 * resets internal components.
 *
 * @param arr Pointer to the array.
 */
template<typename T>
void init(Tundra::DynArr::DynamicArray<T> *arr)
{
    Tundra::DynArr::Internal::underlying_init(arr, 
        Tundra::DynArr::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes an array with a specified capacity. Allocates memory and 
 * resets internal components.
 * 
 * If `init_capacity` is 0, it is set to 2.
 * 
 * @param arr Pointer to the array.
 * @param init_capacity Initial capacity in elements.
 */
template<typename T>
void init(Tundra::DynArr::DynamicArray<T> *arr, uint64_t init_capacity)
{
    // Set the initial capcity to 2 if it is less than 2.
    init_capacity += 2 * (init_capacity == 0);

    Tundra::DynArr::Internal::underlying_init(arr, init_capacity);
}

/**
 * @brief Releases heap memory allocated for the array.
 * 
 * After calling this method, the array should not be used unless 
 * reinitialized.
 * 
 * It is safe to call this method on an array that has already been 
 * deconstructed, or never initialized.
 * 
 * @param arr Pointer to the array.
 */
template<typename T>
void deconstruct(Tundra::DynArr::DynamicArray<T> *arr)
{
    if(arr->data == NULL) return;

    free(arr->data);
    arr->data = NULL;
}

/**
 * @brief Ensures the array has the capacity to store `extra_elements`, 
 * resizing and reallocating if necessary.
 * 
 * @param arr Pointer to the array.
 * @param extra_elements Number of extra elements.
 */
template<typename T>
void reserve_for(Tundra::DynArr::DynamicArray<T> *arr, 
    uint64_t extra_elements)
{
    arr->capacity = (Tundra::reserve_memory((void**)&arr->data, 
        extra_elements * sizeof(T),
        arr->num_elements * sizeof(T),
        arr->capacity * sizeof(T))) / sizeof(T);
}

/**
 * @brief Adds an element to the end of the array, automatically resizing if 
 * needed.
 *  
 * @param arr Pointer to the array. 
 * @param element Pointer to the element.
 */
template<typename T>
void add(Tundra::DynArr::DynamicArray<T> *arr, const T *element)
{
    Tundra::DynArr::Internal::check_and_handle_resize(arr);

    arr->data[arr->num_elements++] = *element;
}

/**
 * @brief Adds multiple elements to the end of the array, automatically resizing
 * if needed.
 * 
 * @param arr Pointer to the array. 
 * @param elements Pointer to an array of elements.
 * @param num_elements Number of elements in `elements`.
 */
template<typename T>
void add_multiple(Tundra::DynArr::DynamicArray<T> *arr, const T* elements, 
    uint64_t num_elements)
{
    Tundra::DynArr::reserve_for(arr, num_elements);

    Tundra::copy_mem((void*)elements, 
        (void*)(arr->data + (arr->num_elements * sizeof(T))), 
        num_elements * sizeof(T));

    arr->num_elements += num_elements;
}

/**
 * @brief Expands the array to hold at least `num_elements` indexable and 
 * modifiable elements.
 * 
 * If `num_elements` is less than or equal to the current number of elements, 
 * the array remains unchanged. Otherwise, the array is resized to allow 
 * indexed access up to `num_elements - 1`. If the internal capacity is 
 * insufficient, it is increased and memory is reallocated as needed.
 * 
 * Newly allocated elements are left uninitialized and must be set manually 
 * before use.
 * 
 * This function does not shrink the array. To reduce the size, use 
 * `shrink_to_capacity`.
 * 
 * @param arr Pointer to the array.
 * @param num_elements Desired total number of elements.
 */
template<typename T>
void resize(Tundra::DynArr::DynamicArray<T> *arr,
    uint64_t num_elements)
{
    if(num_elements <= arr->num_elements) return;

    if(num_elements > arr->capacity)
    {
        Tundra::DynArr::reserve_for(arr, num_elements - arr->capacity);
    }

    arr->num_elements = num_elements;
}

/**
 * @brief Shrinks the array's allocated capacity to the specified value.
 * 
 * If `capacity` is greater than or equal to the current capacity, no changes 
 * are made. If `capacity` is less than the current number of elements, excess 
 * elements are discarded and the array is resized accordingly.
 * 
 * Memory is reallocated if the capacity is reduced.
 * 
 * @param arr Pointer to the array.
 * @param capacity New capacity to shrink to.
 */
template<typename T>
void shrink_to_capacity(Tundra::DynArr::DynamicArray<T> *arr, 
    uint64_t capacity)
{
    if(capacity >= arr->capacity) return;

    Tundra::DynArr::Internal::underlying_shrink(arr, capacity);
}

/**
 * @brief Reduces the array's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 *
 * @param arr Pointer to the array.
 */
template<typename T>
void shrink_to_fit(Tundra::DynArr::DynamicArray<T> *arr)
{
    if(arr->num_elements == arr->capacity) return;

    Tundra::DynArr::Internal::underlying_shrink(arr, arr->num_elements);
}

/**
 * @brief Removes the element at the specified index and shifts subsequent 
 * elements back by one position, returns true if successful.
 *
 * Performs bounds checking on the index.
 *
 * @param array Pointer to the DynamicArray to modify.
 * @param index Index of the element to remove.
 * 
 * @return True if erasure was successful, false otherwise.
 */
template<typename T>
bool erase(Tundra::DynArr::DynamicArray<T> *arr, uint64_t index)
{
    if(index >= arr->num_elements) return false;

     // Copy the elements ahead of the index back one position.
    memmove(arr->data + index, 
        arr->data + index + 1,
        (arr->num_elements - index - 1) * sizeof(T));

    --arr->num_elements;
    return true;
} 

/**
 * @brief Retruns a pointer to the last element in the array of elements.
 * 
 * @attention This method assumes the array is not empty. Calling it with
 * and empty array will result in undefined behavior!
 * 
 * @param arr Pointer to the array to analyze.
 * 
 * @return Pointer to the last element in the array, assuming non zero elements.
 */
template<typename T>
T* back(Tundra::DynArr::DynamicArray<T> *arr)
{
    return &arr->data[arr->num_elements - 1];
}

/**
 * @brief Returns a pointer to the value at an index.
 * 
 * Performs bounds checking on the index, returning NULL if index is invalid.
 * 
 * @param arr Pointer to the array to fetch from.
 * @param index Index into the array.
 * 
 * @return Pointer to the item at the index, or NULL if index is invalid.
 */
template<typename T>
T* at(Tundra::DynArr::DynamicArray<T> *arr, uint64_t index)
{
    if(index >= arr->num_elements) return NULL;

    return arr->data + index;
}

/**
 * @brief Returns a read-only pointer to the value at an index.
 * 
 * Performs bounds checking on the index, returning NULL if index is invalid.
 *  
 * @param arr Pointer to the array.
 * @param index Index into the array.
 * 
 * @return const T* Read-only pointer to the item at the index, or NULL if index
 * is invalid. 
 * 
 */
template<typename T>
const T* peek(const Tundra::DynArr::DynamicArray<T> *arr, uint64_t index)
{
    return arr->data + index;
}

/**
 * @brief Returns the number in elements in the array.
 * 
 * @param arr Pointer to the array to analyze.
 * 
 * @return uint64_t Number of elements in the array. 
 */
template<typename T>
uint64_t size(Tundra::DynArr::DynamicArray<T> *arr)
{
    return arr->num_elements;
}

} // namespace Tundra::DynArr
