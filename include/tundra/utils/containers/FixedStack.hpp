/**
 * @file FixedStack.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */


#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::FxdStk
{

// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
 * Memory for this component is Stack allocated.
 * 
 * The Stack must be initialized using the `init` method(s) before it is used.
 * No `free` call is needed for the FixedStack, since it is Stack 
 * allocated.
 * 
 * Internals are read-only.
 * 
 * @tparam T Data type the Stack stores
 * @tparam cap Capacity of the Stack.
 */
template<typename T, Tundra::uint64 cap>
struct FixedStack 
{
    // Fixed array that stores the elements pushed on the Stack.
    T data[cap];

    // Number of elements on the Stack.
    Tundra::uint64 num_elements;
};


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes the Stack, resetting internal components.
 * 
 * @param stk Pointer to the Stack. 
 */
template<typename T, Tundra::uint64 cap>
inline void init(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Resets the Stack to an empty state.
 * 
 * This does not modify or zero out the underlying memory. Only the element 
 * count is reset to zero, so subsequent pushes will overwrite previous data 
 * from the start of the Stack.
 * 
 * @param stk Pointer to the Stack.
 */
template<typename T, Tundra::uint64 cap>
inline void clear(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Attempts to push an element onto the Stack.
 *
 * If the Stack is full, the push fails and returns false.
 * Otherwise, the element is added to the top of the Stack and returns true.
 *
 * @param stk Pointer to the Stack.
 * @param element Pointer to the element to push.
 * 
 * @return True if the element was successfully pushed, false if the Stack is 
 * full.
 */
template<typename T, Tundra::uint64 cap>
inline bool push(Tundra::FxdStk::FixedStack<T, cap> *stk, const T *element)
{
    if(stk->num_elements >= cap) return false;

    stk->data[stk->num_elements++] = *element;
    return true;
}

/**
 * @brief Pops a value off the top of the Stack.
 * 
 * Performs a size check to ensure the Stack contains elements before popping.
 *
 * Does not return the popped value. Call the `back` method to get a reference
 * to the last element before popping.
 * 
 * @param stk Pointer to the Stack. 
 */
template<typename T, Tundra::uint64 cap>
inline void pop(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements -= 1 * (stk->num_elements > 0);
}

/**
 * @brief Checks whether the Stack contains any elements, returning true if
 * it is empty.
 *
 * @param stk Pointer to the Stack.
 * 
 * @return bool True if the Stack has no elements, false otherwise.
 */
template<typename T, Tundra::uint64 cap>
inline bool is_empty(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return !stk->num_elements;
}

/**
 * @brief Checks if the Stack is full, returning true if it is.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return bool True if the Stack is full, false otherwise.
 */
template<typename T, Tundra::uint64 cap>
inline bool is_full(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return stk->num_elements == cap;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return [Tundra::uint64] Number of elements on the Stack.
 */
template<typename T, Tundra::uint64 cap>
inline Tundra::uint64 size(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return stk->num_elements;
}

/**
 * @brief Returns the fixed capacity of the Stack.
 *  
 * @param stk Pointer to the Stack.
 * 
 * @return [Tundra::uint64] Fixed capacity of the Stack. 
 */
template<typename T, Tundra::uint64 cap>
constexpr Tundra::uint64 capacity(const Tundra::FxdStk::FixedStack<T, cap> *stk) // NOLINT
{
    return cap;
}

/**
 * @brief Returns a pointer to the top element on the Stack.
 * 
 * @attention For fast access, this method does not perform a size on the 
 * Stack. It is the user's responsibility to ensure the Stack is not empty. 
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return [T*] Pointer to the top element of the Stack.
 */
template<typename T, Tundra::uint64 cap>
inline T* back(Tundra::FxdStk::FixedStack<T, cap> *stk)
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
template<typename T, Tundra::uint64 cap>
inline const T* peek_unchecked(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return &stk->data[stk->num_elements - 1];
}

/**
 * @brief Returns a read-only pointer to the top element on the Stack.
 * 
 * Performs a size check on the Stack, returning NULL if it's empty.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return [const T*] Read-only pointer to the top element of the Stack, or NULL
 *    if the Stack is empty.
 */
template<typename T, Tundra::uint64 cap>
inline const T* peek(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return (stk->num_elements) ? &stk->data[stk->num_elements - 1] : NULL;
}

} // namespace Tundra::FxdStk
