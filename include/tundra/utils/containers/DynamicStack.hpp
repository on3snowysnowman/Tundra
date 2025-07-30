/**
 * @file DynamicStack.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container providing LIFO behavior for storing 
 *    elements.
 * @version 0.1
 * @date 07-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>

#include "tundra/utils/memory/MemoryAlloc.hpp"


namespace Tundra::DynStk
{

namespace Internal
{

constexpr uint8_t DEFAULT_ALIGNMENT = 32;

} // namespace Internal

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container providing LIFO behavior for storing 
 * elements.
 *
 * The Stack must be initialized using the `init` method(s) before it is used.
 *
 * Some memory for this component is heap allocated and must be manually freed
 * using the `free` method when the Stack is no longer needed.
 * 
 * @tparam T Data Type the Stack stores.
 * @tparam alignment Alignment in bytes to align the Stack's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<typename T, 
    uint8_t alignment = Tundra::DynStk::Internal::DEFAULT_ALIGNMENT>
struct DynamicStack 
{
    // Heap allocated array of elements on the Stack.
    T* data;

    // Number of elements currently on the Stack.
    uint64_t num_elements;

    // Current maximum number of elements that the Array can store before it 
    // needs to be resized.
    uint64_t capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Default capacity in elements of an Stack.
constexpr uint64_t DEFAULT_CAPACITY = 4;

template<typename T, uint8_t alignment>
inline void underlying_init(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    uint64_t init_capacity)
{
    stk->data = (T*)Tundra::alloc_aligned<alignment>(init_capacity * sizeof(T));
    stk->num_elements = 0;
    stk->capacity = init_capacity;
}

template<typename T, uint8_t alignment>
inline void check_and_handle_resize(
    Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    if(stk->num_elements < stk->capacity) { return; }

    uint64_t new_capacity = 2 * stk->capacity;

    // Get a new memory block that is twice the capacity of the current one.
    T* new_memory = (T*)Tundra::alloc_and_copy_aligned_mem<alignment>(
        (void*)stk->data,
        stk->num_elements * sizeof(T),
        new_capacity * sizeof(T));

    Tundra::free_aligned(stk->data);
    stk->data = new_memory;
    stk->capacity = new_capacity;
}

/**
 * @brief Underlying shrink method, shrinks the Stack's capacity to match the 
 * specified new capacity.
 * 
 * @param stk Pointer to the Stack.
 * @param capacity Capacity to shrink to.
 */
template<typename T, uint8_t alignment>
inline void underlying_shrink(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    uint64_t capacity)
{
    uint64_t new_capacity_bytes = capacity * sizeof(T);

    T* new_memory = (T*)Tundra::alloc_and_copy_aligned_mem<alignment>(stk->data, 
        new_capacity_bytes, new_capacity_bytes);

    Tundra::free_aligned(stk->data);
    stk->data = new_memory;
    stk->capacity = capacity;
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Ensures the Stack has the capacity to store `extra_elements`, 
 * resizing and reallocating if necessary.
 * 
 * @param arr Pointer to the Stack.
 * @param extra_elements Number of extra elements.
 */
template<typename T, uint8_t alignment>
inline void reserve_for(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    uint64_t extra_elements)
{
    stk->capacity = (Tundra::reserve_aligned_mem<alignment>((void**)&stk->data,
        extra_elements * sizeof(T),
        stk->num_elements * sizeof(T),
        stk->capacity * sizeof(T))) / sizeof(T);
}

/**
 * @brief Initializes a Stack with default capacity. Allocates memory and 
 * resets internal components.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T, uint8_t alignment>
inline void init(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    Tundra::DynStk::Internal::underlying_init(stk, 
        Tundra::DynStk::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes a Stack with a specified capacity. Allocates memory and
 * resets internal components.
 *
 * If `init_capacity` is 0, it is set to the default capacity.
 * 
 * @param stk Pointer to the Stack. 
 * @param init_capacity Initial capacity in elements.
 */
template<typename T, uint8_t alignment>
inline void init(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    uint64_t init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) * 
        Tundra::DynStk::Internal::DEFAULT_CAPACITY;

    Tundra::DynStk::Internal::underlying_init(stk, init_capacity);
}

/**
 * @brief Initializes a Stack with an initial array of elements to place on the 
 * Stack. Allocates at least enough memory for `num_elements` and resets 
 * internal components.
 * 
 * @param stk Pointer to the Stack. 
 * @param init_elements Pointer to the array of initial elements to copy in.
 * @param num_elements Number of elements to copy.
 */
template<typename T, uint8_t alignment>
inline void init(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    const T* init_elements, uint64_t num_elements)
{
    uint64_t num_copy_bytes = num_elements * sizeof(T);

    // Temporary var for passing a reference to to the reserve call to get the 
    // new capacity in bytes.
    uint64_t new_capacity_bytes;

    Tundra::alloc_and_reserve_aligned_mem<alignment>((void**)&stk->data,
        &new_capacity_bytes, num_copy_bytes);
    
    Tundra::copy_mem((void*)init_elements, (void*)stk->data,
        num_copy_bytes);

    stk->num_elements = num_elements;
    stk->capacity = new_capacity_bytes / sizeof(T);
}

/**
 * @brief Releases heap memory allocated for the Stack.
 *
 * After calling this method, the Stack should not be used unless reinitialized.
 *
 * It is safe to call this method on a Stack that has already been 
 * freed, or never initialized.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T, uint8_t alignment>
inline void free(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    if(!stk->data) { return; }

    Tundra::free_aligned(stk->data);
    stk->data = NULL;
}

/**
 * @brief Resets the Stack to an empty state.
 *
 * This does not modify, shrink, deallocate or zero out the underlying memory. 
 * Only the element count is reset to zero, so subsequent pushes will overwrite 
 * previous data from the start of the Stack.
 * 
 * @param stk Pointer to the Stack. 
 */
template<typename T, uint8_t alignment>
inline void clear(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Pushes an element onto the Stack, automatically resizing if needed.
 * 
 * @param stk Pointer to the Stack.
 * @param element Read-only pointer to the element.
 */
template<typename T, uint8_t alignment>
inline void push(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    const T* element)
{
    Tundra::DynStk::Internal::check_and_handle_resize(stk);

    stk->data[stk->num_elements++] = *element;
}

/**
 * @brief Pushes an element onto the Stack, automatically resizing if needed.
 * 
 * @param stk Pointer to the Stack.
 * @param element Element to add.
 */
template<typename T, uint8_t alignment>
inline void push(Tundra::DynStk::DynamicStack<T, alignment> *stk,
    const T element)
{
    Tundra::DynStk::Internal::check_and_handle_resize(stk);

    stk->data[stk->num_elements++] = element;
}

/**
 * @brief Pushes multiple elements onto the Stack, automatically resizing if 
 * needed.
 *
 * Note that the last element in `elements` will be the first to be popped off 
 * the Stack.
 * 
 * @param stk Pointer to the Stack.
 * @param elements Pointer to the elements to add.
 * @param num_elements Number of elements in `elements`.
 */
template<typename T, uint8_t alignment>
inline void push_multiple(Tundra::DynStk::DynamicStack<T, alignment> *stk, 
    const T* elements, uint64_t num_elements)
{   
    Tundra::DynStk::reserve_for(stk, num_elements);

    Tundra::copy_mem((void*)elements,
        (void*)(stk->data + stk->num_elements),
        num_elements * sizeof(T));

    stk->num_elements += num_elements;
}

/**
 * @brief Pops a value off the Stack.
 *
 * Performs a size check to ensure the Stack contains elements before popping.
 *
 * Does not return the popped value. Call the `back` method to get a reference 
 * to the last element before popping.
 * 
 * @param stk Pointer to the Stack. 
 */
template<typename T, uint8_t alignment>
inline void pop(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    stk->num_elements -= 1 * (stk->num_elements > 0);
}

/**
 * @brief Shrinks the Stack's allocated capacity to the specified value.
 *
 * If `capacity` is greater than or equal to the current capacity, no changes
 * are made. If `capacity` is less than the current number of elements, excess
 * elements are discarded and the Stack is resized accordingly. 
 * 
 * @param stk Pointer to the Stack.
 * @param new_capacity New capacity to shrink to.
 */
template<typename T, uint8_t alignment>
inline void shrink_to_new_capacity(
    Tundra::DynStk::DynamicStack<T, alignment> *stk, uint64_t new_capacity)
{
    if(new_capacity >= stk->capacity) { return; }

    Tundra::DynStk::Internal::underlying_shrink(stk, new_capacity);
}

/**
 * @brief Shrinks the Stack's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T, uint8_t alignment>
inline void shrink_to_fit(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    if(stk->num_elements == stk->capacity) { return; }

    Tundra::DynStk::Internal::underlying_shrink(stk, stk->num_elements);
}

/**
 * @brief Checks whether the Stack contains any elements, returning true if
 * it is empty.
 * 
 * @param stk 
 * @return [bool] True if the Stack is empty, false otherwise.
 */
template<typename T, uint8_t alignment>
inline bool is_empty(const Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return !stk->num_elements;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Pointer to the Stack. 
 * @return [uint64_t] Number of elements on the Stack. 
 */
template<typename T, uint8_t alignment>
inline uint64_t size(const Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return stk->num_elements;
}

/**
 * @brief Returns the current capacity of the Stack.
 * 
 * @param stk Pointer to the Stack.  
 * @return [uint64_t] Current capacity of the Stack.
 */
template<typename T, uint8_t alignment>
inline uint64_t capacity(const Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return stk->capacity;
}

/**
 * @brief Returns a pointer to the top element on the Stack.
 *
 * @attention For fast access, this method does not perform a size check on the 
 * Stack. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return [T*] Pointer to the top element of the Stack. 
 */
template<typename T, uint8_t alignment>
inline T* back(Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return &stk->data[stk->num_elements - 1];
}

/**
 * @brief Returns a read-only pointer to the top element on the Stack.
 * 
 * @attention For fast access, this method does not perform a size check on the 
 * Stack. It is the user's responsibility to ensure the Stack is not empty.
 *
 * @param stk Pointer to the Stack.
 *
 * @return [const T*] Read-only pointer to the top element of the Stack. 
 */
template<typename T, uint8_t alignment>
inline const T* peek_unchecked(
    const Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return stk->data[stk->num_elements - 1];
}

/**
 * @brief Returns a read-only pointer to the top element on the Stack.
 * 
 * Performs a size check on the Stack, returning NULL if it's empty.
 *
 * @param stk Pointer to the Stack.
 *
 * @return [const T*] Read-only pointer to the top element of the Stack. 
 */
template<typename T, uint8_t alignment>
inline const T* peek(
    const Tundra::DynStk::DynamicStack<T, alignment> *stk)
{
    return (stk->num_elements) ? stk->data[stk->num_elements - 1] : NULL;
}


} // namespace Tundra::DynStk
