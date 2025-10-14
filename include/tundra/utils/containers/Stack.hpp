/**
 * @file Stack.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2025
 */


#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/FatalHandler.hpp"


namespace Tundra::Stk
{

// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
 * Memory for this component is Stack allocated.
 * 
 * The Stack must be initialized using the `init` method(s) before it is used.
 * No `free` call is needed for the Stack, since it is Stack 
 * allocated.
 * 
 * Internals are read-only.
 * 
 * @tparam T Data type the Stack stores
 * @tparam cap Capacity of the Stack.
 */
template<typename T, Tundra::uint64 cap>
struct Stack 
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
inline void init(Tundra::Stk::Stack<T, cap> &stk)
{
    stk.num_elements = 0;
}

/**
 * @brief Creates a Stack with an initial list of elements.
 * 
 * Do note that `args` is added to the Stack in order, and therefore the 
 * last element in `args` will be the first popped off the stack.
 *
 * Evaluated at compile time.
 * 
 * @param args Elements to fill Stack with.
 *
 * @return consteval Tundra::Stk::Stack<T, sizeof...(Ts)> Created stack with 
 *    initial elements. 
 */
template<typename T, typename... Ts>
consteval Tundra::Stk::Stack<T, sizeof...(Ts)> make(Ts&&... args)
{
    return Tundra::Stk::Stack<T, sizeof...(Ts)>
        { { T(args)... }, sizeof...(Ts) };
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
inline void clear(Tundra::Stk::Stack<T, cap> &stk)
{
    stk.num_elements = 0;
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
inline bool push(Tundra::Stk::Stack<T, cap> &stk, const T *element)
{
    if(stk.num_elements >= cap) { return false; }

    stk.data[stk.num_elements++] = *element;
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
inline void pop(Tundra::Stk::Stack<T, cap> &stk)
{
    if(stk.num_elements > 0) 
    {
        --stk.num_elements;
        return;
    }

    TUNDRA_FATAL("Attempted to pop but Stack was empty.");
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
inline bool is_empty(const Tundra::Stk::Stack<T, cap> &stk)
{
    return !stk.num_elements;
}

/**
 * @brief Checks if the Stack is full, returning true if it is.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return bool True if the Stack is full, false otherwise.
 */
template<typename T, Tundra::uint64 cap>
inline bool is_full(const Tundra::Stk::Stack<T, cap> &stk)
{
    return stk.num_elements == cap;
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
template<typename T, Tundra::uint64 cap>
inline T* front(Tundra::Stk::Stack<T, cap> &stk)
{
    return stk.data + stk.num_elements - 1;
}

template<typename T, Tundra::uint64 cap>
inline const T* front(const Tundra::Stk::Stack<T, cap> &stk)
{
    return stk.data + stk.num_elements - 1;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Pointer to the Stack.
 * 
 * @return [Tundra::uint64] Number of elements on the Stack.
 */
template<typename T, Tundra::uint64 cap>
inline Tundra::uint64 size(const Tundra::Stk::Stack<T, cap> &stk)
{
    return stk.num_elements;
}

/**
 * @brief Returns the fixed capacity of the Stack.
 *  
 * @param stk Pointer to the Stack.
 * 
 * @return [Tundra::uint64] Fixed capacity of the Stack. 
 */
template<typename T, Tundra::uint64 cap>
consteval Tundra::uint64 capacity(const Tundra::Stk::Stack<T, cap> &stk) // NOLINT
{
    return cap;
}

} // namespace Tundra::Stk
