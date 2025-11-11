/**
 * @file DynamicArray.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added 
 *    elements.
 * @version 0.1
 * @date 2025-07-16
 * 
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/memory/MemUtils.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/Math.hpp"

namespace Tundra::DynArr
{

namespace Internal
{

// Default capacity in elements of an Array.
constexpr Tundra::uint64 DEFAULT_CAPACITY = 4;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally added 
 *    elements.
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
template<typename T>
struct DynamicArray
{
    // TUNDRA_CHECK_ALIGNMENT(alignment);

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

/**
 * @brief Underlying initialization method. Allocates initial 
 * memory for `init_capacity` elements and sets internal values.
 * 
 * @param arr Pointer to the Array.
 * @param init_capacity Initial capacity in elements to allocate.
 */
template<typename T>
inline bool underlying_init(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 init_capacity)
{ 
    arr.data = reinterpret_cast<T*>(
        Tundra::alloc_mem(init_capacity * sizeof(T)));

    if(arr.data == nullptr) { return false; }

    arr.num_elements = 0;
    arr.capacity = init_capacity;
    return true;
}

/**
 * @brief Checks if the Array has filled its allocated capacity, resizing if it
 * has.
 *  
 * @param arr Pointer to the Array.
 */
template<typename T>
inline bool check_and_handle_resize(
    Tundra::DynArr::DynamicArray<T> &arr)
{
    if(arr.num_elements < arr.capacity) { return true; }

    Tundra::uint64 new_capacity = 2 * arr.capacity;

    // Get a new memory block that is twice the capacity of the current one and
    // copy over the old bytes.
    T *new_mem = reinterpret_cast<T*>(Tundra::alloc_copy_mem(
        arr.data, 
        new_capacity * sizeof(T),
        arr.num_elements * sizeof(T)));

    if(new_mem == nullptr)
    {
        return false;
    }

    Tundra::free_mem(arr.data);
    arr.data = new_mem;
    arr.capacity = new_capacity;
    return true;
}

/**
 * @brief Underlying shrink method, shrinks the Array's capacity to match the 
 * specified new capacity.
 *  
 * @param arr Pointer to the Array.
 * @param capacity Capacity to shrink to.
 */
template<typename T>
inline bool underlying_shrink(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 capacity)
{
    Tundra::uint64 new_capacity_bytes = capacity * sizeof(T);
    
    T *new_mem = reinterpret_cast<T*>(Tundra::alloc_copy_mem(arr.data, 
        new_capacity_bytes, new_capacity_bytes));

    if(new_mem == nullptr)
    {
        return false;
    }

    Tundra::free_mem(arr.data);
    arr.data = new_mem;
    arr.capacity = capacity;

    // If the capacity was shrunk smaller than the existing elements, update
    // the number of elements. 
    arr.num_elements = Tundra::clamp_max(arr.num_elements, arr.capacity);

    return true;
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
template<typename T>
inline void reserve_for(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 extra_elements)
{
    Tundra::uint64 cap_bytes = arr.capacity * sizeof(T);

    Tundra::reserve_mem(
        &arr.data,
        &cap_bytes,
        arr.num_elements * sizeof(T),
        extra_elements * sizeof(T));

    arr.capacity = cap_bytes / sizeof(T);

    // arr.capacity = 
    // (Tundra::reserve_mem
    // (
    //     (void**)&arr.data, 
    //     extra_elements * sizeof(T),
    //     arr.num_elements * sizeof(T),
    //     arr.capacity * sizeof(T)
    // )
    // ) / sizeof(T);

    // return arr.data != nullptr;

}

/**
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * resets internal components.
 *
 * @param arr Pointer to the Array.
 */
template<typename T>
inline bool init(Tundra::DynArr::DynamicArray<T> &arr)
{
    return Tundra::DynArr::Internal::underlying_init(arr, 
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
template<typename T>
inline bool  init(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) * 
        Tundra::DynArr::Internal::DEFAULT_CAPACITY;

    return Tundra::DynArr::Internal::underlying_init(arr, init_capacity);
}

/**
 * @brief Initializes an Array with an initial array of elements. Allocates 
 * at least enough memory for `num_elements` and resets internal components.
 *
 * Elements are copied in from `init_elements`.
 *  
 * @param arr Pointer to the Array. 
 * @param init_elements Read-only pointer to the array of initial elements.
 * @param num_elements Number of elements to copy.
 */
template<typename T>
inline bool init(Tundra::DynArr::DynamicArray<T> &arr,
    const T *init_elements, Tundra::uint64 num_elements)
{
    Tundra::uint64 num_copy_bytes = num_elements * sizeof(T);

    // Temporary var for passing a reference to to the reserve call to get the 
    // new capacity in bytes.
    Tundra::uint64 new_capacity_bytes;

    Tundra::alloc_reserve_mem(reinterpret_cast<void**>(&arr.data),
        &new_capacity_bytes, num_copy_bytes);

    if(arr.data == nullptr) { return false; }

    Tundra::copy_mem_fwd(reinterpret_cast<void*>(init_elements), 
        reinterpret_cast<void*>(arr.data),
        num_copy_bytes);

    arr.num_elements = num_elements;
    arr.capacity = new_capacity_bytes / sizeof(T);
    return true;
}

/**
 * @brief Releases heap memory allocated for the Array.
 * 
 * After calling this method, the Array should not be used unless reinitialized.
 * 
 * It is safe to call this method on an Array that has already been freed, or 
 * never initialized.
 * 
 * @param arr Pointer to the Array.
 */
template<typename T>
inline void free(Tundra::DynArr::DynamicArray<T> &arr)
{
    Tundra::free_mem(arr.data);
    arr.data = NULL;
}

template<typename T>
inline bool copy(Tundra::DynArr::DynamicArray<T> &dst, 
    const Tundra::DynArr::DynamicArray<T> &src)
{
    // Both objects are the same.
    if(&dst == &src) { return true; }

    const Tundra::uint64 SRC_CAP_BYTES = src.capacity * sizeof(T);

    if(dst.capacity != src.capacity || dst.data == nullptr)
    {
        T *new_mem = reinterpret_cast<T*>(
            Tundra::alloc_mem(SRC_CAP_BYTES));
        if(new_mem == nullptr) { return false; }

        free(dst);
        dst.data = new_mem;
        dst.capacity = src.capacity;
    }

    Tundra::copy_mem_fwd(src.data, dst.data, SRC_CAP_BYTES);
    dst.num_elements = src.num_elements;
    return true;
}

template<typename T>
inline void move(Tundra::DynArr::DynamicArray<T> &dst, 
    Tundra::DynArr::DynamicArray<T> &&src)
{
    // Both objects are the same.
    if(&dst == &src) { return; }

    free(dst);

    dst = src;
    src.data = nullptr;
}

/**
 * @brief Resets the Array to an empty state.
 *
 * This does not modify, shrink, deallocate or zero out the underlying memory. 
 * Only the element count is reset to zero, so subsequent adds will overwrite 
 * previous data from the start of the Array.
 * 
 * @param arr Pointer to the Array. 
 */
template<typename T>
inline void clear(Tundra::DynArr::DynamicArray<T> &arr)
{
    arr.num_elements = 0;
}

/**
 * @brief Adds an element to the end of the Array, automatically resizing if 
 * needed.
 *  
 * @param arr Pointer to the Array. 
 * @param element Element to add.
 */
template<typename T>
inline bool add(Tundra::DynArr::DynamicArray<T> &arr, 
    const T &element)
{
    if(!Tundra::DynArr::Internal::check_and_handle_resize(arr)) 
        { return false; }

    arr.data[arr.num_elements++] = element;
    return true;
}

/**
 * @brief Adds multiple elements to the end of the Array, automatically resizing
 * if needed.
 * 
 * @param arr Pointer to the Array. 
 * @param elements Pointer to the elements to add.
 * @param num_elements Number of elements in `elements`.
 */
template<typename T>
inline bool add_multiple(Tundra::DynArr::DynamicArray<T> &arr, 
    const T* elements, Tundra::uint64 num_elements)
{
    if(!Tundra::DynArr::reserve_for(arr, num_elements)) { return false; }

    Tundra::copy_mem_fwd(elements, 
        (arr.data + arr.num_elements), 
        num_elements * sizeof(T));

    arr.num_elements += num_elements;
    return true;
}

template<typename T>
inline void insert(Tundra::DynArr::DynamicArray<T> &arr, 
    const T &element, Tundra::uint64 index)
{
    if(index > arr.num_elements)
    {
        // Invalid index.
        TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
            arr.num_elements);
    }

    if(index == arr.num_elements)
    {
        Tundra::DynArr::add(arr, element);
        return;
    }

    const Tundra::uint64 NUM_CPY_BYTES = arr.num_elements - index;

    if(arr.num_elements != arr.capacity)
    {
        // Simply copy the elements at and after the index forward one.
        Tundra::copy_mem_fwd(arr.data + index, arr.data + index + 1, 
            NUM_CPY_BYTES);
        arr.data[index] = element;
        ++arr.num_elements;
        return;
    }

    // -- We need a new mem block. --

    // New capacity in number of elements.
    const Tundra::uint64 NEW_CAP = 2 * arr.capacity;

    T *new_mem = reinterpret_cast<T*>(
        Tundra::alloc_mem(NEW_CAP * sizeof(T)));

    if(new_mem == nullptr)
    {
        TUNDRA_FATAL("malloc returned nullptr.");
    }

    // Copy bytes before the index to insert.
    Tundra::copy_mem_fwd(arr.data, new_mem, index * sizeof(T));

    // Insert element
    new_mem[index] = element;

    // Copy bytes after the index to insert.
    Tundra::copy_mem_fwd(arr.data + index, new_mem + index + 1, 
        (arr.num_elements - index) * sizeof(T));

    Tundra::free_mem(arr.data);
    ++arr.num_elements;
    arr.data = new_mem;
    arr.capacity = NEW_CAP;
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
template<typename T>
inline bool resize(Tundra::DynArr::DynamicArray<T> &arr,
    Tundra::uint64 num_elements)
{
    if(num_elements <= arr.num_elements) { return true; }

    if(num_elements > arr.capacity)
    {
        Tundra::DynArr::reserve_for(arr, num_elements - arr.capacity);

        if(arr.data == nullptr) { return false; }
    }

    arr.num_elements = num_elements;
    return true;
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
template<typename T>
inline bool shrink_to_new_capacity(
    Tundra::DynArr::DynamicArray<T> &arr, Tundra::uint64 new_capacity)
{
    if(new_capacity >= arr.capacity) { return true; }

    return Tundra::DynArr::Internal::underlying_shrink(arr, new_capacity);
}

/**
 * @brief Shrinks the Array's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 *
 * @param arr Pointer to the Array.
 */
template<typename T>
inline bool shrink_to_fit(Tundra::DynArr::DynamicArray<T> &arr)
{
    if(arr.num_elements == arr.capacity) { return true; }

    return Tundra::DynArr::Internal::underlying_shrink(arr, arr.num_elements);
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
template<typename T>
inline bool erase(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 index)
{
    if(index >= arr.num_elements) { return false; }

    if(index == arr.num_elements - 1) 
    {
        --arr.num_elements; 
        return true; 
    }

    if(index == arr.num_elements - 2)
    {
        --arr.num_elements;
        // Only need to copy a single element.
        arr.data[index] = arr.data[arr.num_elements];
        return true;
    }

    Tundra::erase_shift_bytes(reinterpret_cast<void*>(arr.data), 
        index * sizeof(T), sizeof(T), arr.num_elements * sizeof(T));

    --arr.num_elements;

    return true;
} 

template<typename T>
inline T* front(Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.data[0];
}

template<typename T>
inline const T* front(const Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.data[0];
}

/**
 * @brief Retruns a pointer to the last element in the Array
 * 
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Pointer to the Array.
 * 
 * @return T* Pointer to the last element in the Array.
 */
template<typename T>
inline T* back(Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.data[arr.num_elements - 1];
}

template<typename T>
inline const T* back(const Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.data[arr.num_elements - 1];
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
 * @return T* Pointer to the item at the index.
 */
template<typename T>
inline T& at_unchecked(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 index)
{
    return arr.data[index];
}

template<typename T>
inline const T& at_unchecked(const Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a pointer to the value at an index with bounds checking.
 * 
 * Performs bounds checking on `index`, returning NULL if it's invalid.
 * 
 * @param arr Pointer to the Array.
 * @param index Index into the Array.
 * 
 * @return T* Pointer to the item at the index, or NULL if index is invalid.
 */
template<typename T>
inline T& at(Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 index)
{
    if(index < arr.num_elements) { return arr.data[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
        arr.num_elements);
}

template<typename T>
inline const T& at(const Tundra::DynArr::DynamicArray<T> &arr, 
    Tundra::uint64 index)
{
    if(index < arr.num_elements) { return arr.data[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
        arr.num_elements);
}

/**
 * @brief Returns the number of elements in the Array.
 * 
 * @param arr Read-only pointer to the Array to analyze.
 * 
 * @return Tundra::uint64 Number of elements in the Array. 
 */
template<typename T>
inline Tundra::uint64 size(
    const Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.num_elements;
}

/**
 * @brief Returns the current capacity of the Array.
 * 
 * @param arr Read-only pointer to the Array. 
 * @return Tundra::uint64 Current capacity of the Array. 
 */
template<typename T>
inline Tundra::uint64 capacity(const Tundra::DynArr::DynamicArray<T> &arr)
{
    return arr.capacity;
}

} // namespace Tundra::DynArr
