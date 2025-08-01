/**
 * @file DynamicArray.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for storing procedurally added elements.
 * @version 0.1
 * @date 2025-07-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/memory/MemoryAlloc.hpp"
#include "tundra/utils/memory/MemoryUtils.hpp"


namespace Tundra::DynArr
{

namespace Internal
{

// Default memory alignment.
constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for storing procedurally added elements.
 * 
 * The Array must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `free` method when the Array is no longer required.
 * 
 * Internals are read-only.
 * 
 * @tparam T Data Type the Array stores.
 * @tparam alignment Alignment in bytes to align the Array's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<typename T, 
    Tundra::uint8 alignment = Tundra::DynArr::Internal::DEFAULT_ALIGNMENT>
struct DynamicArray
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

    // Heap allocated array of elements.
    T* data;

    // Number of elements currently in the Array.
    Tundra::uint64 num_elements;

    // Current maximum number of elements that the Array can store before it 
    // needs to be resized.
    Tundra::uint64 capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Default capacity in elements of an Array.
constexpr Tundra::uint64 DEFAULT_CAPACITY = 4;

/**
 * @brief Underlying initialization method. Allocates initial 
 * memory for `init_capacity` elements and sets internal values.
 * 
 * @param arr Pointer to the Array.
 * @param init_capacity Initial capacity in elements to allocate.
 */
template<typename T, Tundra::uint8 alignment>
inline void underlying_init(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 init_capacity)
{ 
    arr->data = (T*)Tundra::alloc_aligned<alignment>(init_capacity * sizeof(T));
    arr->num_elements = 0;
    arr->capacity = init_capacity;
}

/**
 * @brief Checks if the Array has filled its allocated capacity, resizing if it
 * has.
 *  
 * @param arr Pointer to the Array.
 */
template<typename T, Tundra::uint8 alignment>
inline void check_and_handle_resize(
    Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    if(arr->num_elements < arr->capacity) { return; }

    Tundra::uint64 new_capacity = 2 * arr->capacity;

    // Get a new memory block that is twice the capacity of the current one.
    T *new_memory = (T*)Tundra::alloc_and_copy_aligned_mem<alignment>(
        (void*)arr->data, 
        arr->num_elements * sizeof(T),
        new_capacity * sizeof(T));

    Tundra::free_aligned(arr->data);
    arr->data = new_memory;
    arr->capacity = new_capacity;
}

/**
 * @brief Underlying shrink method, shrinks the Array's capacity to match the 
 * specified new capacity.
 *  
 * @param arr Pointer to the Array.
 * @param capacity Capacity to shrink to.
 */
template<typename T, Tundra::uint8 alignment>
inline void underlying_shrink(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 capacity)
{
    Tundra::uint64 new_capacity_bytes = capacity * sizeof(T);
    
    T* new_memory = (T*)Tundra::alloc_and_copy_aligned_mem<alignment>(arr->data, 
        new_capacity_bytes, new_capacity_bytes);

    Tundra::free_aligned(arr->data);
    arr->data = new_memory;
    arr->capacity = capacity;
}

} // namespace Internal


// Public ----------------------------------------------------------------------

/**
 * @brief Ensures the Array has the capacity to store `extra_elements`, 
 * resizing and reallocating if necessary.
 * 
 * @param arr Pointer to the Array.
 * @param extra_elements Number of extra elements.
 */
template<typename T, Tundra::uint8 alignment>
inline void reserve_for(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 extra_elements)
{
    arr->capacity = (Tundra::reserve_aligned_mem<alignment>((void**)&arr->data, 
        extra_elements * sizeof(T),
        arr->num_elements * sizeof(T),
        arr->capacity * sizeof(T))) / sizeof(T);
}

/**
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * resets internal components.
 *
 * @param arr Pointer to the Array.
 */
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    Tundra::DynArr::Internal::underlying_init(arr, 
        Tundra::DynArr::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes an Array with a specified capacity. Allocates memory and 
 * resets internal components.
 * 
 * If `init_capacity` is 0, it is set to the default capacity.
 * 
 * @param arr Pointer to the Array.
 * @param init_capacity Initial capacity in elements.
 */
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) * 
        Tundra::DynArr::Internal::DEFAULT_CAPACITY;

    Tundra::DynArr::Internal::underlying_init(arr, init_capacity);
}

/**
 * @brief Initializes an Array with an initial array of elements. Allocates 
 * at least enough memory for `num_elements` and resets internal components.
 *
 * Elements are copied in from `init_elements`.
 *  
 * @param arr Pointer to the Array. 
 * @param init_elements Pointer to the array of initial elements to copy in.
 * @param num_elements Number of elements to copy.
 */
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::DynArr::DynamicArray<T, alignment> *arr,
    const T* init_elements, Tundra::uint64 num_elements)
{
    Tundra::uint64 num_copy_bytes = num_elements * sizeof(T);

    // Temporary var for passing a reference to to the reserve call to get the 
    // new capacity in bytes.
    Tundra::uint64 new_capacity_bytes;

    Tundra::alloc_and_reserve_aligned_mem<alignment>((void**)&arr->data,
        &new_capacity_bytes, num_copy_bytes);

    Tundra::copy_mem((void*)init_elements, (void*)arr->data,
        num_copy_bytes);

    arr->num_elements = num_elements;
    arr->capacity = new_capacity_bytes / sizeof(T);
}

/**
 * @brief Releases heap memory allocated for the Array.
 * 
 * After calling this method, the Array should not be used unless reinitialized.
 * 
 * It is safe to call this method on an Array that has already been 
 * freed, or never initialized.
 * 
 * @param arr Pointer to the Array.
 */
template<typename T, Tundra::uint8 alignment>
inline void free(Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    if(!arr->data) { return; }

    Tundra::free_aligned(arr->data);
    arr->data = NULL;
}

/**
 * @brief Resets the Array to an empty State.
 *
 * This does not modify, shrink, deallocate or zero out the underlying memory. 
 * Only the element count is reset to zero, so subsequent adds will overwrite 
 * previous data from the start of the Array.
 * 
 * @param arr Pointer to the Array. 
 */
template<typename T, Tundra::uint8 alignment>
inline void clear(Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    arr->num_elements = 0;
}

/**
 * @brief Adds an element to the end of the Array, automatically resizing if 
 * needed.
 *  
 * @param arr Pointer to the Array. 
 * @param element Read-only pointer to the element.
 */
template<typename T, Tundra::uint8 alignment>
inline void add(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    const T *element)
{
    Tundra::DynArr::Internal::check_and_handle_resize(arr);

    arr->data[arr->num_elements++] = *element;
}

/**
 * @brief Adds an element to the end of the Array, automatically resizing if 
 * needed.
 *  
 * @param arr Pointer to the Array. 
 * @param element Element to add.
 */
template<typename T, Tundra::uint8 alignment>
inline void add(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    const T element)
{
    Tundra::DynArr::Internal::check_and_handle_resize(arr);

    arr->data[arr->num_elements++] = element;
}

/**
 * @brief Adds multiple elements to the end of the Array, automatically resizing
 * if needed.
 * 
 * @param arr Pointer to the Array. 
 * @param elements Pointer to the elements to add.
 * @param num_elements Number of elements in `elements`.
 */
template<typename T, Tundra::uint8 alignment>
inline void add_multiple(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    const T* elements, Tundra::uint64 num_elements)
{
    Tundra::DynArr::reserve_for(arr, num_elements);

    Tundra::copy_mem((void*)elements, 
        (void*)(arr->data + arr->num_elements), 
        num_elements * sizeof(T));

    arr->num_elements += num_elements;
}

/**
 * @brief Expands the Array to hold at least `num_elements` indexable and 
 * modifiable elements.
 * 
 * If `num_elements` is less than or equal to the current number of elements, 
 * the Array remains unchanged. Otherwise, the Array is resized to allow 
 * indexed access up to `num_elements - 1`. If the internal capacity is 
 * insufficient, it is increased and memory is reallocated as needed.
 * 
 * Newly allocated elements are left uninitialized.
 * 
 * This method does not shrink the Array. To reduce the size, use one of the 
 * shrink methods.
 * 
 * @param arr Pointer to the Array.
 * @param num_elements Desired total number of elements.
 */
template<typename T, Tundra::uint8 alignment>
inline void resize(Tundra::DynArr::DynamicArray<T, alignment> *arr,
    Tundra::uint64 num_elements)
{
    if(num_elements <= arr->num_elements) { return; }

    if(num_elements > arr->capacity)
    {
        Tundra::DynArr::reserve_for(arr, num_elements - arr->capacity);
    }

    arr->num_elements = num_elements;
}

/**
 * @brief Shrinks the Array's allocated capacity to the specified value.
 * 
 * If `capacity` is greater than or equal to the current capacity, no changes 
 * are made. If `capacity` is less than the current number of elements, excess 
 * elements are discarded and the Array is resized accordingly.
 * 
 * Memory is reallocated if the capacity is reduced.
 * 
 * @param arr Pointer to the Array.
 * @param new_capacity New capacity to shrink to.
 */
template<typename T, Tundra::uint8 alignment>
inline void shrink_to_new_capacity(
    Tundra::DynArr::DynamicArray<T, alignment> *arr, Tundra::uint64 new_capacity)
{
    if(new_capacity >= arr->capacity) { return; }

    Tundra::DynArr::Internal::underlying_shrink(arr, new_capacity);
}

/**
 * @brief Shrinks the Array's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 *
 * @param arr Pointer to the Array.
 */
template<typename T, Tundra::uint8 alignment>
inline void shrink_to_fit(Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    if(arr->num_elements == arr->capacity) { return; }

    Tundra::DynArr::Internal::underlying_shrink(arr, arr->num_elements);
}

/**
 * @brief Removes the element at the specified index and shifts subsequent 
 * elements back by one position, returns true if successful.
 *
 * Performs bounds checking on the index.
 *
 * @param arr Pointer to the Array.
 * @param index Index of the element to remove.
 * 
 * @return bool True if erasure was successful, false otherwise.
 */
template<typename T, Tundra::uint8 alignment>
inline bool erase(Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 index)
{
    if(index >= arr->num_elements) { return false; }

    if(index == arr->num_elements - 1) 
    {
        --arr->num_elements; 
        return true; 
    }

    if(index == arr->num_elements - 2)
    {
        --arr->num_elements;
        // Only need to copy a single element.
        arr->data[index] = arr->data[arr->num_elements];
        return true;
    }

    Tundra::erase_and_shift_bytes((void*)arr->data, index * sizeof(T), 
        sizeof(T), arr->num_elements * sizeof(T));

    --arr->num_elements;

    return true;
} 

/**
 * @brief Retruns a pointer to the last element in the Array
 * 
 * Assumes the Array is not empty. Calling it with
 * and empty Array will return junk values.
 * 
 * @param arr Pointer to the Array.
 * 
 * @return [T*] Pointer to the last element in the Array.
 */
template<typename T, Tundra::uint8 alignment>
inline T* back(Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    // return &arr->data[arr->num_elements - 1];
    return arr->data + arr->num_elements - 1;
}

/**
 * @brief Returns a pointer to the value at an index.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid. 
 * 
 * @param arr Pointer to the Array.
 * @param index Index into the Array.
 * 
 * @return [T*] Pointer to the item at the index.
 */
template<typename T, Tundra::uint8 alignment>
inline T* at_unchecked(Tundra::DynArr::DynamicArray<T, alignment> *arr, Tundra::uint64 index)
{
    return arr->data + index;
}

/**
 * @brief Returns a pointer to the value at an index with bounds checking.
 * 
 * Performs bounds checking on `index`, returning NULL if it's invalid.
 * 
 * @param arr Pointer to the Array.
 * @param index Index into the Array.
 * 
 * @return [T*] Pointer to the item at the index, or NULL if index is invalid.
 */
template<typename T, Tundra::uint8 alignment>
inline T* at(Tundra::DynArr::DynamicArray<T, alignment> *arr, Tundra::uint64 index)
{
    return (index < arr->num_elements) ? arr->data + index : NULL;
}

/**
 * @brief Returns a read-only pointer to the value at an index.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure index is valid. 
 *  
 * @param arr Pointer to the Array.
 * @param index Index into the Array.
 * 
 * @return [const T*] Read-only pointer to the item at the index.
 */
template<typename T, Tundra::uint8 alignment>
inline const T* peek_unchecked(const Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 index)
{
    return arr->data + index;
}

/**
 * @brief Returns a read-only pointer to the value at an index with bounds
 * checking.
 * 
 * Performs bounds checking on the index, returning NULL if it's invalid.
 *  
 * @param arr Pointer to the Array.
 * @param index Index into the Array.
 * 
 * @return [const T*] Read-only pointer to the item at the index, or NULL if 
 * index is invalid. 
 */
template<typename T, Tundra::uint8 alignment>
inline const T* peek(const Tundra::DynArr::DynamicArray<T, alignment> *arr, 
    Tundra::uint64 index)
{
    return (index < arr->num_elements) ? arr->data + index : NULL;
}

/**
 * @brief Returns the memory alignment of the Array.
 * 
 * @param arr Pointer to the Array. 
 *
 * @return [Tundra::uint8] Memory alignment of the Array. 
 */
template<typename T, Tundra::uint8 alignment>
constexpr Tundra::uint8 get_alignment(
    const Tundra::DynArr::DynamicArray<T, alignment> *arr) // NOLINT
{
    return alignment;
}

/**
 * @brief Returns the number in elements in the Array.
 * 
 * @param arr Pointer to the Array to analyze.
 * 
 * @return [Tundra::uint64] Number of elements in the Array. 
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 size(const Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    return arr->num_elements;
}

/**
 * @brief Returns the current capacity of the Array.
 * 
 * @param arr Pointer to the Array. 
 * @return [Tundra::uint64] Current capacity of the Array. 
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 capacity(const Tundra::DynArr::DynamicArray<T, alignment> *arr)
{
    return arr->capacity;
}

} // namespace Tundra::DynArr
