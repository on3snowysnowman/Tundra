/**
 * @file DynamicStack.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container providing LIFO behavior for storing 
 * procedurally added elements.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/memory/MemUtils.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/Math.hpp"


namespace Tundra::DynStk
{

// Definitions -----------------------------------------------------------------

namespace Internal
{

// Default capacity in elements of a Stack.
constexpr uint64 DEF_CAP = 4;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container providing LIFO behavior for storing 
 * procedurally added elements.
 *
 * Must be initialized using either the `init`, `copy`, or `move` methods before 
 * use. Must be freed on end of use using the `free` method.
 *
 * Internals are read-only.
 * 
 * @tparam T Element type.
 */
template<typename T>
struct DynamicStack
{
    // Heap allocated array of elements on the Stack.
    T *data;

    // Number of elements on the Stack.
    uint64 num_elem;

    // Current maximum number of elements that can be pushed on the Stack before
    // it needs to be resized.
    uint64 cap;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` elements and sets container components.
 * 
 * @param stk Stack to initialize.
 * @param init_cap Initial capacity in elements.
 */
template<typename T>
inline void internal_init(DynamicStack<T> &stk, uint64 init_cap)
{
    free_mem(stk.data);

    stk.data = static_cast<T*>(alloc_mem(init_cap * sizeof(T)));

    stk.num_elem = 0;
    stk.cap = init_cap;
}

/**
 * @brief Checks if a Stack has filled its allocated capacity, expanding and 
 * reallocating if it has.
 * 
 * @param stk Stack to handle. 
 */
template<typename T>
inline void check_handle_expansion(DynamicStack<T> &stk)
{
    if(stk.num_elem < stk.cap) { return; }

    // Double previous capacity.
    const uint64 NEW_CAP_ELEM = 2 * stk.cap;

    free_mem(stk.data);
    
    stk.data = static_cast<T*>(alloc_copy_mem(
        stk.data,
        NEW_CAP_ELEM * sizeof(T),
        stk.num_elem * sizeof(T)
    ));
    stk.cap = NEW_CAP_ELEM;
}

/**
 * @brief Ensures the Stack has the capacity to store `extra_elem` elements, 
 * expanding and reallocating if needed.
 * 
 * @param arr Stack to handle. 
 * @param extra_elem Number of extra elements.
 */
template<typename T>
inline void reserve_for(DynamicStack<T> &stk, uint64 extra_elem)
{
    uint64 cap_bytes = stk.cap * sizeof(T);

    reserve_mem(
        reinterpret_cast<void**>(&stk.data),
        &cap_bytes,
        stk.num_elem * sizeof(T),
        extra_elem * sizeof(T)
    );

    stk.cap = cap_bytes / sizeof(T);
}

/**
 * @brief Internal shrink method, reallocates the Stack to a capacity of `cap`.
 * 
 * @param stk Stack to shrink. 
 * @param cap Capacity to shrink to.
 */
template<typename T>
inline void internal_shrink(DynamicStack<T> &stk, uint64 cap)
{
    const uint64 CAP_BYTE = cap * sizeof(T);

    T *new_mem = static_cast<T*>(alloc_copy_mem(stk.data, CAP_BYTE, CAP_BYTE));

    free_mem(stk.data);
    stk.data = new_mem;
    stk.cap = cap;

    // If the capacity was shrunk smaller than the existing elements, update the 
    // number of elements.
    stk.num_elem = clamp_max(stk.num_elem, stk.cap);
}

} // namespace Internal


/**
 * @brief Initializes a Stack with default capacity. Allocates memory and 
 * sets internal components.
 * 
 * @param stk Stack to init.
 */
template<typename T>
inline void init(DynamicStack<T> &stk)
{
    Internal::internal_init(stk, Internal::DEF_CAP);
}

/**
 * @brief Initialize a Stack with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Stack is initialized with default capacity.
 * 
 * @param stk Stack to init.
 * @param init_cap Specified initial capacity.
 */
template<typename T>
inline void init(DynamicStack<T> &stk, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? Internal::DEF_CAP : init_cap;

    Internal::internal_init(stk, init_cap);
}

/**
 * @brief Initializes a Stack with initial elements. Allocates memory and set 
 * internal components.
 *
 * `elements` are copied into the Stack. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated for the Stack. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will be
 * allocated to optimize against immediate reallocation on the next add request.
 * 
 * The last element in `elements` will be the element on the top of the Stack.
 *
 * @param stk Stack to init. 
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
template<typename T>
inline void init(DynamicStack<T> &stk, const T *elements, uint64 num_elem, 
    bool strict_alloc = false)
{
    free_mem(stk.data);

    const uint64 NUM_CPY_BYTE = num_elem * sizeof(T);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        stk.data = static_cast<T*>(alloc_mem(NUM_CPY_BYTE));
        copy_mem_fwd(elements, stk.data, NUM_CPY_BYTE);
        stk.num_elem = num_elem;
        stk.cap = num_elem;
        return;
    }

    // -- Use the "reserving" method to alloc, which will generally alloc more
    // space than is needed to prevent immediate expansion on next push call. --

    // Temp var for retrieving the capacity of the allocated block through the
    // next call. Capacity in bytes, not elements.
    uint64 temp_cap_bytes;

    alloc_reserve_mem(
        reinterpret_cast<void**>(&stk.data),
        &temp_cap_bytes,
        NUM_CPY_BYTE
    );

    copy_mem_fwd(elements, stk.data, NUM_CPY_BYTE);

    stk.num_elem = num_elem;
    stk.cap = temp_cap_bytes / sizeof(T);
}

/**
 * @brief Frees memory allocated for a Stack.
 *
 * After calling this method, the Stack must not be used unless reinitialized.
 *
 * It is safe to call this method on a Stack that has already been freed, or 
 * never initialized.
 * 
 * @param stk Stack to free. 
 */
template<typename T>
inline void free(DynamicStack<T> &stk)
{
    free_mem(stk.data);
    stk.data = nullptr;
}

/**
 * @brief Deep copies `src` to `dst`.
 * 
 * If the Stacks are of the same addres, nothing is done.
 *
 * `dst` can be an uninitialized Stack.
 * 
 * @param src Stack to source from. 
 * @param dst Stack to deep copy to, can be uninitialized.
 */
template<typename T>
inline void copy(const DynamicStack<T> &src, DynamicStack<T> &dst)
{
    if(&dst == &src) { return; }

    const uint64 SRC_CAP_BYTE = src.cap * sizeof(T);

    if(dst.cap != src.cap || dst.data == nullptr)
    {
        free_mem(dst.data);
        dst.data = static_cast<T*>(alloc_mem(SRC_CAP_BYTE));
        dst.cap = src.cap;
    }

    copy_mem_fwd(src.data, dst.data, SRC_CAP_BYTE);
    dst.num_elem = src.num_elem;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.
 *
 * If the Stacks are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Stack.
 * 
 * @param src Stack to source from. 
 * @param dst Stack to transfer resources to, can be uninitialized.
 */
template<typename T>
inline void move(DynamicStack<T> &src, DynamicStack<T> &dst)
{
    if(&dst == &src) { return; }
    
    free_mem(dst.data);
    dst = src;
    src.data = nullptr;
}

/**
 * @brief Resets the Stack to an empty state.
 *
 * Does not modify, shrink, deallocate or zero out underlying memory. Only the 
 * element count is reset to zero so subsequent adds will overwrite previous 
 * data from the top of the Stack.
 * 
 * @param stkStack to clear. 
 */
template<typename T>
inline void clear(DynamicStack<T> &stk)
{
    stk.num_elem = 0;
}

/**
 * @brief Pushes a copy of `elem` onto the Stack, automatically resizing if 
 * needed.
 * 
 * @param stk Stack to push to.
 * @param elem Element to copy.
 */
template<typename T>
inline void push(DynamicStack<T> &stk, const T &elem)
{
    Internal::check_handle_expansion(stk);

    stk.data[stk.num_elem++] = elem;
}

/**
 * @brief Pops a value off the Stack.
 *
 * Performs a size check to ensure the Stack contains elements before popping.
 * A fatal is thrown if the Stack contains no elements with the Stack 
 * unmodified.
 *
 * Does not return the popped value. Call the `top` method to get a reference to 
 * the last element before popping.
 * 
 * @param stk Stack to pop.
*/
template<typename T>
inline void pop(DynamicStack<T> &stk)
{
    if(stk.num_elem == 0)
    {
        TUNDRA_FATAL("Attempted to pop but Stack was empty.");
        return;
    }

    --stk.num_elem;
}

/**
 * @brief Expands the Stack to hold at least `num_elem` indexable and modifiable
 * elements.
 * 
 * If `num_elem` is less than or equal to the current number of elements, this
 * method does not shrink the Stack. To reduce the size, use one of the shrink 
 * methods.
 *  
 * @param stk Stack to resize. 
 * @param num_elem Desired total number of elements.
 */
template<typename T>
inline void resize(DynamicStack<T> &stk, uint64 num_elem)
{
    if(num_elem <= stk.num_elem) { return; }

    if(num_elem > stk.cap)
    {
        Internal::reserve_for(stk, num_elem - stk.num_elem);
    }

    stk.num_elem = num_elem;
}

/**
 * @brief Shrinks the Stack's allocated capacity to a specified capacity.
 * 
 * If `new_cap` is greater than or equal to the current capacity, the Stack is 
 * not modified. If `capacity` is less than the current number of elements,
 * excess elements are discarded and the Stack is resized and reallocated to the 
 * value specified.
 * 
 * @param stk Stack to shrink. 
 * @param new_cap Capacity to shrink to.
 */
template<typename T>
inline void shrink_to_new_cap(DynamicStack<T> &stk, uint64 new_cap)
{
    if(new_cap >= stk.cap) { return; }

    Internal::internal_shrink(stk, new_cap);
}

/**
 * @brief Shrinks the Stack's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param stk Stack to shrink.
 */
template<typename T>
inline void shrink_to_fit(DynamicStack<T> &stk)
{
    if(stk.num_elem == stk.cap) { return; }

    Internal::internal_shrink(stk, stk.num_elem);
}

/**
 * @brief Returns true if the Stack contains no elements.
 * 
 * @param stk Stack to check.
 * 
 * @return bool True if Stack is empty. 
 */
template<typename T>
inline bool is_empty(const DynamicStack<T> &stk)
{
    return stk.num_elem == 0;
}

/**
 * @brief Returns a reference to the top element on the Stack.
 * 
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 *
 * @param stk Stack to get top of. 
 *
 * @return T& Reference to the top element. 
 */
template<typename T>
inline T& top(DynamicStack<T> &stk)
{
    return stk.data[stk.num_elem - 1];
}

/**
 * @brief Returns a const-reference to the top element on the Stack.
 * 
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 *
 * @param stk Stack to get top of. 
 *
 * @return const T& Const-reference to the top element. 
 */
template<typename T>
inline const T& top(const DynamicStack<T> &stk)
{
    return stk.data[stk.num_elem - 1];
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Stack to get size of.
 * 
 * @return uint64 Number of elements on the Stack.
 */
template<typename T>
inline uint64 size(DynamicStack<T> &stk)
{
    return stk.num_elem;
}

/**
 * @brief Returns the current capacity of the Stack.
 * 
 * @param stk Stack to get capacity of.
 * 
 * @return uint64 Capacity of the Stack.
 */
template<typename T>
inline uint64 capacity(DynamicStack<T> &stk)
{
    return stk.cap;
}

} // namespace Tundra::DynStk
