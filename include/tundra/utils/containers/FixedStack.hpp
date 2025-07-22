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

#include <stdint.h>


namespace Tundra::FxdStk
{

// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size container providing LIFO behavior for storing elements.
 * 
 * Memory for this component is stack allocated.
 * 
 * The stack must be initialized using the `init` method(s) before it is used.
 * No `deconstruct` call is needed for the FixedStack, since it is stack 
 * allocated.
 * 
 * Internals are read-only.
 * 
 * @tparam T Data type the stack stores
 * @tparam cap Capacity of the stack.
 */
template<typename T, uint64_t cap>
struct FixedStack 
{
    // Fixed array that stores the elements pushed on the stack.
    T data[cap];

    // Number of elements on the stack.
    uint64_t num_elements;
};


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes the stack, resetting internal components.
 * 
 * @param stk Pointer to the stack. 
 */
template<typename T, uint64_t cap>
void init(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Resets the stack to an empty state.
 * 
 * This does not modify or zero out the underlying memory. Only the element 
 * count is reset to zero, so subsequent pushes will overwrite previous data 
 * from the start of the stack.
 * 
 * @param stk Pointer to the stack.
 */
template<typename T, uint64_t cap>
void clear(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements = 0;
}

/**
 * @brief Pops a value off the top of the stack.
 * 
 * Does not return the popped value. Call the `back` method to get a reference
 * to the last element before popping.
 * 
 * @param stk Pointer to the stack. 
 */
template<typename T, uint64_t cap>
void pop(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    stk->num_elements -= 1 * (stk->num_elements > 0);
}

/**
 * @brief Attempts to push an element onto the stack.
 *
 * If the stack is full, the push fails and returns false.
 * Otherwise, the element is added to the top of the stack and returns true.
 *
 * @param stk Pointer to the stack.
 * @param element Pointer to the element to push.
 * 
 * @return True if the element was successfully pushed, false if the stack is 
 * full.
 */
template<typename T, uint64_t cap>
bool push(Tundra::FxdStk::FixedStack<T, cap> *stk, const T *element)
{
    if(stk->num_elements >= cap) return false;

    stk->data[stk->num_elements++] = *element;
    return true;
}

/**
 * @brief Checks whether the stack contains any elements, returning true if
 * it is empty.
 *
 * @param stk Pointer to the stack.
 * 
 * @return True if the stack has no elements, false otherwise.
 */
template<typename T, uint64_t cap>
bool is_empty(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return !stk->num_elements;
}

/**
 * @brief Checks if the stack is full, returning true if it is.
 * 
 * @param stk Pointer to the stack.
 * 
 * @return True if the stack is full, false otherwise.
 */
template<typename T, uint64_t cap>
bool is_full(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return stk->num_elements == cap;
}

/**
 * @brief Returns the number of elements on the stack.
 * 
 * @param stk Pointer to the stack.
 * 
 * @return uint64_t Number of elements on the stack.
 */
template<typename T, uint64_t cap>
uint64_t size(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return stk->num_elements;
}

/**
 * @brief Returns the fixed capacity of the stack.
 *  
 * @param stk Pointer to the stack.
 * 
 * @return uint64_t Fixed capacity of the stack. 
 */
template<typename T, uint64_t cap>
constexpr uint64_t capacity(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return cap;
}

/**
 * @brief Returns a pointer to the top element on the stack.
 * 
 * @attention The stack must not be empty when calling this function, otherwise,
 * behavior is undefined.
 * 
 * @param stk Pointer to the stack.
 * 
 * @return T* Pointer to the top element of the stack.
 */
template<typename T, uint64_t cap>
T* back(Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return &stk->data[stk->num_elements - 1];
}

/**
 * @brief Returns a read only pointer to the top element on the stack.
 * 
 * @attention The stack must not be empty when calling this function, otherwise,
 * behavior is undefined.
 * 
 * @param stk Pointer to the stack.
 * 
 * @return const T* Read-only pointer to the top element of the stack.
 */
template<typename T, uint64_t cap>
const T* peek(const Tundra::FxdStk::FixedStack<T, cap> *stk)
{
    return &stk->data[stk->num_elements - 1];
}

} // namespace Tundra::FxdStk
