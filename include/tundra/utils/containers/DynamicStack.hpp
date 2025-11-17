/**
 * @file DynamicStack.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container providing LIFO behavior for storing 
 *    elements.
 * @version 0.1
 * @date 07-27-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/memory/MemUtils.hpp"


namespace Tundra::DynStk
{

namespace Internal
{

// Default capacity in elements of an Stack.
constexpr Tundra::uint64 DEFAULT_CAPACITY = 4;

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
template<typename T>
struct DynamicStack 
{
    // Heap allocated array of elements on the Stack.
    T* data;

    // Number of elements currently on the Stack.
    Tundra::uint64 num_elements;

    // Current maximum number of elements that the Array can store before it 
    // needs to be resized.
    Tundra::uint64 capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

template<typename T>
inline bool underlying_init(Tundra::DynStk::DynamicStack<T> &stk,
    Tundra::uint64 init_capacity)
{
    stk.data = reinterpret_cast<T*>(
        Tundra::alloc_mem(init_capacity * sizeof(T)));

    if(stk.data == nullptr) { return false; }

    stk.num_elements = 0;
    stk.capacity = init_capacity;
    return true;
}

template<typename T>
inline bool check_and_handle_resize(Tundra::DynStk::DynamicStack<T> &stk)
{
    if(stk.num_elements < stk.capacity) { return true; }

    Tundra::uint64 new_capacity = 2 * stk.capacity;

    // Get a new memory block that is twice the capacity of the current one.
    T* new_mem = reinterpret_cast<T*>(Tundra::alloc_copy_mem(
        stk.data,
        new_capacity * sizeof(T), 
        stk.num_elements * sizeof(T)));

    if(new_mem == nullptr) { return false; }

    Tundra::free_mem(stk.data);
    stk.data = new_mem;
    stk.capacity = new_capacity;
    return true;
}

/**
 * @brief Underlying shrink method, shrinks the Stack's capacity to match the 
 * specified new capacity.
 * 
 * @param stk Pointer to the Stack.
 * @param capacity Capacity to shrink to.
 */
template<typename T>
inline bool underlying_shrink(Tundra::DynStk::DynamicStack<T> &stk,
    Tundra::uint64 capacity)
{
    Tundra::uint64 new_capacity_bytes = capacity * sizeof(T);

    T* new_mem = reinterpret_cast<T*>(Tundra::alloc_copy_mem(stk.data, 
        new_capacity_bytes, new_capacity_bytes));

    if(new_mem == nullptr) { return false; }

    Tundra::free_mem(stk.data);
    stk.data = new_mem;
    stk.capacity = capacity;
    return true;
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
template<typename T>
inline bool reserve_for(Tundra::DynStk::DynamicStack<T> &stk,
    Tundra::uint64 extra_elements)
{
    Tundra::uint64 cap_bytes = stk.capacity * sizeof(T);

    Tundra::reserve_mem(
        &stk.data, 
        &cap_bytes, 
        stk.num_elements * sizeof(T), 
        extra_elements * sizeof(T));

    stk.capacity = cap_bytes / sizeof(T);


    // stk.capacity = (Tundra::reserve_mem((void**)&stk.data,
    //     extra_elements * sizeof(T),
    //     stk.num_elements * sizeof(T),
    //     stk.capacity * sizeof(T))) / sizeof(T);

    return stk.data != nullptr;
}

/**
 * @brief Initializes a Stack with default capacity. Allocates memory and 
 * resets internal components.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T>
inline bool init(Tundra::DynStk::DynamicStack<T> &stk)
{
    return Tundra::DynStk::Internal::underlying_init(stk, 
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
template<typename T>
inline bool init(Tundra::DynStk::DynamicStack<T> &stk,
    Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) * 
        Tundra::DynStk::Internal::DEFAULT_CAPACITY;

    return Tundra::DynStk::Internal::underlying_init(stk, init_capacity);
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
template<typename T>
inline bool init(Tundra::DynStk::DynamicStack<T> &stk,
    const T* init_elements, Tundra::uint64 num_elements)
{
    Tundra::uint64 num_copy_bytes = num_elements * sizeof(T);

    // Temporary var for passing a reference to to the reserve call to get the 
    // new capacity in bytes.
    Tundra::uint64 new_capacity_bytes;

    Tundra::alloc_reserve_mem(&stk.data,
        &new_capacity_bytes, num_copy_bytes);
    
    if(stk.data == nullptr) { return false; }

    Tundra::copy_mem_fwd(init_elements, stk.data,
        num_copy_bytes);

    stk.num_elements = num_elements;
    stk.capacity = new_capacity_bytes / sizeof(T);
    return true;
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
template<typename T>
inline void free(Tundra::DynStk::DynamicStack<T> &stk)
{
    Tundra::free_mem(stk.data);
    stk.data = nullptr;
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
template<typename T>
inline void clear(Tundra::DynStk::DynamicStack<T> &stk)
{
    stk.num_elements = 0;
}

/**
 * @brief Pushes an element onto the Stack, automatically resizing if needed.
 * 
 * @param stk Pointer to the Stack.
 * @param element Element to add.
 */
template<typename T>
inline bool push(Tundra::DynStk::DynamicStack<T> &stk,
    const T& element)
{
    if(!Tundra::DynStk::Internal::check_and_handle_resize(stk))
        { return false; }

    stk.data[stk.num_elements++] = element;
    return true;
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
template<typename T>
inline bool push_multiple(Tundra::DynStk::DynamicStack<T> &stk, 
    const T* elements, Tundra::uint64 num_elements)
{   
    if(!Tundra::DynStk::reserve_for(stk, num_elements)) { return false; }

    Tundra::copy_mem_fwd((void*)elements,
        (void*)(stk.data + stk.num_elements),
        num_elements * sizeof(T));

    stk.num_elements += num_elements;
    return true;
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
template<typename T>
inline void pop(Tundra::DynStk::DynamicStack<T> &stk)
{
    if(stk.num_elements > 0)
    {
        --stk.num_elements; 
        return;
    }

    // Stack was empty. 
    TUNDRA_FATAL("Attempted to pop but Stack was empty.");
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
template<typename T>
inline bool shrink_to_new_capacity(
    Tundra::DynStk::DynamicStack<T> &stk, Tundra::uint64 new_capacity)
{
    if(new_capacity >= stk.capacity) { return true; }

    return Tundra::DynStk::Internal::underlying_shrink(stk, new_capacity);
}

/**
 * @brief Shrinks the Stack's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T>
inline bool shrink_to_fit(Tundra::DynStk::DynamicStack<T> &stk)
{
    if(stk.num_elements == stk.capacity) { return true; }

    return Tundra::DynStk::Internal::underlying_shrink(stk, stk.num_elements);
}

/**
 * @brief Checks whether the Stack contains any elements, returning true if
 * it is empty.
 * 
 * @param stk 
 * @return bool True if the Stack is empty, false otherwise.
 */
template<typename T>
inline bool is_empty(const Tundra::DynStk::DynamicStack<T> &stk)
{
    return !stk.num_elements;
}

/**
 * @brief Returns a pointer to the top element on the Stack.
 *
 * @attention For fast access, this method does not perform a size check on the 
 * Stack. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return T* Pointer to the top element of the Stack. 
 */
template<typename T>
inline T* front(Tundra::DynStk::DynamicStack<T> &stk)
{
    return stk.data[stk.num_elements - 1];
}

template<typename T>
inline const T* front(const Tundra::DynStk::DynamicStack<T> &stk)
{
    return stk.data[stk.num_elements - 1];
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Pointer to the Stack. 
 * @return [Tundra::uint64] Number of elements on the Stack. 
 */
template<typename T>
inline Tundra::uint64 size(const Tundra::DynStk::DynamicStack<T> &stk)
{
    return stk.num_elements;
}

/**
 * @brief Returns the current capacity of the Stack.
 * 
 * @param stk Pointer to the Stack.  
 * @return [Tundra::uint64] Current capacity of the Stack.
 */
template<typename T>
inline Tundra::uint64 capacity(const Tundra::DynStk::DynamicStack<T> &stk)
{
    return stk.capacity;
}

} // namespace Tundra::DynStk
