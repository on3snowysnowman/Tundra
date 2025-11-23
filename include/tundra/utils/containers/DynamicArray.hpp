/**
 * @file DynamicArray.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elements.
 * @date 2025-11-20
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/memory/MemUtils.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/Math.hpp"
#include <iostream>

namespace Tundra::DynArr
{


// Definitions -----------------------------------------------------------------

namespace Internal 
{

// Default capacity in elements of an Array.
constexpr Tundra::uint64 DEF_CAP = 4;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally added
 * elements.
 *
 * Must be initialized using either the init, copy or move methods before use. 
 * Must be freed on end of use using the `free` method.
 *
 * Internals are read-only.
 * 
 * @tparam T Element type.
 */
template<typename T>
struct DynamicArray
{
    // Heap allocated array of elements.
    T *data; 

    // Number of elements currently added to the Array.
    Tundra::uint64 num_elem;

    // Current maximum number of elements that can be added to the Array before 
    // it needs to be resized.
    Tundra::uint64 cap;
};

/**
 * @brief Random-access iterator for DynamicArray.
 *
 * Provides pointer-like operations (dereference, increment/decrement,
 * comparisons) for traversing and accessing elements.
 *
 * @tparam T Element type.
 */
template<typename T>
struct Iterator
{
    // Pointer to an element in an Array.
    T *datum;
};

/**
 * @brief Random-access const iterator for DynamicArray.
 *
 * Provides pointer-like operations (dereference, increment/decrement,
 * comparisons) for traversing and accessing elements.
 *
 * @tparam T Element type.
 */
template<typename T>
struct ConstIterator
{
    // Const pointer to an element in an Array.
    const T *datum;
};


// Internal Methods ------------------------------------------------------------

namespace Internal
{

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` elements and sets container components.
 * 
 * @param arr Reference of the Array to initialize. 
 * @param init_cap Initial capacity in elements.
 */
template<typename T>
inline void internal_init(DynamicArray<T> &arr, Tundra::uint64 init_cap)
{
    Tundra::free_mem(arr.data);
    arr.data = reinterpret_cast<T*>(
        Tundra::alloc_mem(init_cap * sizeof(T)));
    
    arr.num_elem = 0;
    arr.cap = init_cap;
}

/**
 * @brief Checks if an Array has filled its allocated capacity, expanding and
 * reallocating if it has.
 * 
 * @param arr Array to handle. 
 */
template<typename T>
inline void check_handle_expansion(DynamicArray<T> &arr)
{
    if(arr.num_elem < arr.cap) { return; }

    // Double previous capacity.
    const Tundra::uint64 NEW_CAP_ELEM = 2 * arr.cap;

    Tundra::free_mem(arr.data);

    arr.data = static_cast<T*>(Tundra::alloc_copy_mem(
            arr.data, 
            NEW_CAP_ELEM * sizeof(T),
            arr.num_elem * sizeof(T))
        );
    arr.cap = NEW_CAP_ELEM;
}

/**
 * @brief Ensures the Array has the capacity to store `extra_elem` elements,
 * expanding and reallocating if needed.
 * 
 * @param arr Array to handle. 
 * @param extra_elem Number of extra elements.
 */
template<typename T>
inline void reserve_for(DynamicArray<T> &arr, Tundra::uint64 extra_elem)
{
    Tundra::uint64 cap_bytes = arr.cap * sizeof(T);

    Tundra::reserve_mem(
        reinterpret_cast<void**>(&arr.data),
        &cap_bytes,
        arr.num_elem * sizeof(T),
        extra_elem * sizeof(T)
    );

    arr.cap = cap_bytes / sizeof(T);
}

template<typename T>
inline void internal_shrink(DynamicArray<T> &arr, uint64 cap)
{
    const uint64 CAP_BYTE = cap * sizeof(T);

    T *new_mem = static_cast<T*>(alloc_copy_mem(arr.data, CAP_BYTE, CAP_BYTE));

    free_mem(arr.data);
    arr.data = new_mem;
    arr.cap = cap;

    // If the capacity was shrunk smaller than the existing elements, update the
    // number of elements.
    arr.num_elem = clamp_max(arr.num_elem, arr.cap);
}

}; // namespace Internal


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * sets internal components.
 * 
 * @param arr Array to init. 
 */
template<typename T>
inline void init(DynamicArray<T> &arr)
{
    Internal::internal_init(arr, Internal::DEF_CAP);
}

/**
 * @brief Initialize an Array with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Array is initialized with default capacity.
 * 
 * @param arr Array to init, 
 * @param init_cap Specified initial capacity.
 */
template<typename T>
inline void init(DynamicArray<T> &arr, Tundra::uint64 init_cap)
{
    init_cap = (init_cap == 0) ? Internal::DEF_CAP : init_cap; 

    Internal::internal_init(arr, init_cap);
}

/**
 * @brief Initializes an Array with initial elements. Allocates memory and sets
 * internal components.
 *
 * `elements` are copied into the Array. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated to the array. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 * 
 * @tparam T 
 * @param arr Array to init. 
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elemnets in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
template<typename T>
inline void init(DynamicArray<T> &arr, const T *elements, 
    Tundra::uint64 num_elem, bool strict_alloc = false)
{
    Tundra::free_mem(arr.data);

    const Tundra::uint64 NUM_CPY_BYTE = num_elem * sizeof(T);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        arr.data = reinterpret_cast<T*>(Tundra::alloc_mem(NUM_CPY_BYTE));
        Tundra::copy_mem_fwd(elements, arr.data, NUM_CPY_BYTE);
        arr.num_elem = num_elem;
        arr.cap = num_elem;
        return;
    }

    // -- Use the "reserving" method to alloc, which will generally alloc more 
    // space than is needed to prevent immediate expansion on next add call. --

    // Temp var for retrieving the capacity of the allocated block through the
    // next call. Capacity in bytes, not elements.
    Tundra::uint64 temp_cap_bytes;

    Tundra::alloc_reserve_mem(
        reinterpret_cast<void**>(&arr.data), 
        &temp_cap_bytes, 
        NUM_CPY_BYTE);

    Tundra::copy_mem_fwd(elements, arr.data, NUM_CPY_BYTE);

    arr.num_elem = num_elem;
    arr.cap = temp_cap_bytes / sizeof(T);
}

/**
 * @brief Frees memory allocated for an Array.
 *
 * After calling this method, the Array must not be used unless reinitialized.
 *
 * It is safe to call this method with an Array that has already been freed, or
 * never initialized.
 * 
 * @param arr Array to free.
 */
template<typename T>
inline void free(DynamicArray<T> &arr)
{
    Tundra::free_mem(arr.data);
    arr.data = nullptr;
}

/**
 * @brief Deep copies `src` to `dst`.
 *
 * If the Arrays are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Array. 
 * 
 * @param src Array to source from. 
 * @param dst Array to deep copy to, can be uninitialized. 
 */
template<typename T>
inline void copy(const DynamicArray<T> &src, DynamicArray<T> &dst)
{
    if(&dst == &src) { return; }

    const Tundra::uint64 SRC_CAP_BYTE = src.cap * sizeof(T);

    if(dst.cap != src.cap || dst.data == nullptr)
    {
        Tundra::free_mem(dst.data);
        dst.data = static_cast<T*>(Tundra::alloc_mem(SRC_CAP_BYTE));
        dst.cap = src.cap;
    }

    Tundra::copy_mem_fwd(src.data, dst.data, SRC_CAP_BYTE);
    dst.num_elem = src.num_elem;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.
 *
 * If the Arrays are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Array.
 * 
 * @param src Array to source from. 
 * @param dst Array to transfer resources to, can be uninitialized.
 */
template<typename T>
inline void move(DynamicArray<T> &src, DynamicArray<T> &dst)
{
    if(&dst == &src) { return; }

    Tundra::free_mem(dst.data);
    dst = src;
    src.data = nullptr;
}

/**
 * @brief Resets the Array to an empty state.
 *
 * Does not modify, shrink, deallocate or zero out underlying memory. Only the 
 * element count is reset to zero so subsequent adds will overwrite previous 
 * data from the start of the Array.
 * 
 * @param arr Array to clear. 
 */
template<typename T>
inline void clear(DynamicArray<T> &arr)
{
    arr.num_elem = 0;
}

/**
 * @brief Adds a copy of `elem` to the back of the Array, automatically resizing 
 * if needed.
 *  
 * @param arr Array to add to.
 * @param elem Element to copy.
 */
template<typename T>
inline void add(DynamicArray<T> &arr, const T &elem)
{
    Internal::check_handle_expansion(arr);

    arr.data[arr.num_elem++] = elem;
}

/**
 * @brief Copies multiple elements to the end of the Array, automatically 
 * resizing if needed.
 * 
 * Reserves memory beforehand, optimizing over individual adds.
 *
 * @param arr Array to add to.
 * @param elements Array of elements ot copy in.
 * @param num_elem Number of elements in `elements`.
 */
template<typename T>
inline void add_multiple(DynamicArray<T> &arr, const T *elements, 
    Tundra::uint64 num_elem)
{
    Internal::reserve_for(arr, num_elem);

    Tundra::copy_mem_fwd(elements, arr.data + arr.num_elem, 
        num_elem * sizeof(T));
    arr.num_elem += num_elem;
}

/**
 * @brief Inserts a copy of an element at an index, shifting all elements ahead
 * of it forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to insert into.
 * @param element Element to copy.
 * @param index Index to insert.
 */
template<typename T>
inline void insert(DynamicArray<T> &arr, const T &element, Tundra::uint64 index)
{
    if(index > arr.num_elem)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
            arr.num_elem);
        return;
    }

    // If there is space for another element, no grow is needed.
    if(arr.num_elem != arr.cap)
    {
        // Copy the bytes at the index forward 1.
        Tundra::copy_mem_bwd(arr.data + index, arr.data + index + 1, 
            (arr.num_elem - index) * sizeof(T));
        arr.data[index] = element;
        ++arr.num_elem;
        return;
    }

    const Tundra::uint64 NEW_CAP_ELEM = arr.cap * 2;

    // -- We don't have enough space, we need a new block --.
    T *new_mem = static_cast<T*>(Tundra::alloc_mem(NEW_CAP_ELEM * sizeof(T)));

    // Copy bytes before the insertion index.
    Tundra::copy_mem_bwd(arr.data, new_mem, index * sizeof(T));

    // Insert element.
    new_mem[index] = element;

    // Copy bytes after the index.
    Tundra::copy_mem_bwd(arr.data + index + 1, new_mem + index + 1, 
        (arr.num_elem - index) * sizeof(T));

    Tundra::free_mem(arr.data);
    arr.data = new_mem;
    arr.cap = NEW_CAP_ELEM;
    ++arr.num_elem;
}

/**
 * @brief Inserts a copy of an element at an Iterator position, shifting all 
 * elements ahead of it forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to insert into.
 * @param element Element to copy.
 * @param it Iterator to insert element at.
 */
template<typename T>
inline void insert(DynamicArray<T> &arr, const T &element, 
    const Iterator<T> &it)
{
    insert(arr, element, it.datum - arr.data);
}

/**
 * @brief Inserts a copy of an element at a ConstIterator position, shifting all 
 * elements ahead of it forward by one.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to insert into.
 * @param element Element to copy.
 * @param it ConstIterator to insert element at.
 */
template<typename T>
inline void insert(DynamicArray<T> &arr, const T &element,
    const ConstIterator<T> &it)
{
    insert(arr, element, it.datum - arr.data);
}

/**
 * @brief Expands the Array to hold at least `num_elem` indexable and modifiable
 * elements.
 *
 * If `num_elem` is less than or equal to the current number of elements, this 
 * method does not shrink the Array. To reduce the size, use one of the shrink
 * methods.
 *
 * Newly expanded elements are left uninitialized.
 *  
 * @param arr Array to resize.
 * @param num_elem Desired total number of elements.
 */
template<typename T>
inline void resize(DynamicArray<T> &arr, uint64 num_elem)
{
    if(num_elem <= arr.num_elem) { return; }

    if(num_elem > arr.cap)
    {
        Internal::reserve_for(arr, num_elem - arr.num_elem);
    }

    arr.num_elem = num_elem;
}

/**
 * @brief Shrinks the Array's allocated capacity to a specified capacity.
 *
 * If `new_cap` is greater than or equal to the current capacity, the Array is
 * not modified. If `capacity` is less than the current number of elements, 
 * excess elements are discarded and the Array is resized to the value 
 * specified.
 *
 * Memory is reallocated if the capacity is reduced. 
 *
 * @param arr Array to shrink. 
 * @param new_cap Capacity to shrink to.
 */
template<typename T>
inline void shrink_to_new_cap(DynamicArray<T> &arr, uint64 new_cap)
{
    if(new_cap >= arr.cap) { return; }

    Internal::internal_shrink(arr, new_cap);
}

/**
 * @brief Shrinks the Array's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param arr Array to shrink. 
 */
template<typename T>
inline void shrink_to_fit(DynamicArray<T> &arr)
{
    if(arr.num_elem == arr.cap) { return; }

    Internal::internal_shrink(arr, arr.num_elem);
}

/**
 * @brief Removes the element at an index and shifts subsequent elements back by
 * one position.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified.
 * 
 * @param arr Array to modify. 
 * @param index Index to erase.
 */
template<typename T>
inline void erase(DynamicArray<T> &arr, uint64 index)
{
    if(index >= arr.num_elem) 
    { 
        TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
            arr.num_elem);
        return; 
    }

    erase_shift_bytes(reinterpret_cast<void*>(arr.data),
        index * sizeof(T), sizeof(T), arr.num_elem * sizeof(T));

    --arr.num_elem;
}

/**
 * @brief Removes the element at an Iterator position and shifts subsequent 
 * elements back by one position.
 *
 * A fatal is thrown if the Iterator is out of range with the Array unmodified.
 * 
 * @param arr Array to modify. 
 * @param it Iterator to erase at.
 */
template<typename T>
inline void erase(DynamicArray<T> &arr, const Iterator<T> &it)
{
    erase(arr, it.datum - arr.data);
}

/**
 * @brief Removes the element at a ConstIterator position and shifts subsequent 
 * elements back by one position.
 *
 * A fatal is thrown if the Iterator is out of range with the Array unmodified.
 * 
 * @param arr Array to modify. 
 * @param it Iterator to erase at.
 */
template<typename T>
inline void erase(DynamicArray<T> &arr, const ConstIterator<T> &it)
{
    erase(arr, it.datum - arr.data);
}

/**
 * @brief Returns a reference to the first element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array.
 * 
 * @return T& Reference to the first element.
 */
template<typename T>
inline T& front(DynamicArray<T> &arr)
{
    return arr.data[0];
}

/**
 * @brief Returns a const-reference to the first element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array.
 * 
 * @return const T& Const-reference to the first element.
 */
template<typename T>
inline const T& front(const DynamicArray<T> &arr)
{
    return arr.data[0];
}

/**
 * @brief Returns a reference to the last element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array.
 * 
 * @return T& Reference to the last element.
 */
template<typename T>
inline T&  back(DynamicArray<T> &arr)
{
    return arr.data[arr.num_elem - 1];
}

/**
 * @brief Returns a const-reference to the last element of the Array.
 *
 * @attention For fast access, this method does not perform a check if the Array
 * is empty. It is the user's responsibility to ensure the Array is not empty.
 * 
 * @param arr Array.
 * 
 * @return const T& Const-reference to the last element.
 */
template<typename T>
inline const T& back(const DynamicArray<T> &arr)
{
    return arr.data[arr.num_elem - 1];
}

/**
 * @brief Returns a reference to the element at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid. 
 * 
 * @param arr Array to index into.
 * @param index Index to get element.
 *
 * @return Reference to the element at `index`.
 */
template<typename T>
inline T& at_nocheck(DynamicArray<T> &arr, uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a const-reference to the element at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid. 
 * 
 * @param arr Array to index into.
 * @param index Index to get element.
 *
 * @return const T& Const-reference to the element at `index`.
 */
template<typename T>
inline const T& at_nocheck(const DynamicArray<T> &arr, uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a reference to the element at `index`. Performs bounds 
 * checking on `index`.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified. If 
 * the fatal returns, the return value of this method is not defined.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return T& Reference to the element at `index`.
 */
template<typename T>
inline T& at(DynamicArray<T> &arr, uint64 index)
{
    if(index >= arr.num_elem)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
            arr.num_elem);
    }

    return arr.data[index];
}

/**
 * @brief Returns a const-reference to the element at `index`. Performs bounds 
 * checking on `index`.
 *
 * A fatal is thrown if the index is out of range with the Array unmodified. If 
 * the fatal returns, the return value of this method is not defined.
 * 
 * @param arr Array to index into. 
 * @param index Index to get element.
 *
 * @return const T& Const-reference to the element at `index`.
 */
template<typename T>
inline const T& at(const DynamicArray<T> &arr, uint64 index)
{
    if(index >= arr.num_elem)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array size is: \"%llu\".", index, 
            arr.num_elem);
    }

    return arr.data[index];
}

/**
 * @brief Returns the number of elements in the Array.
 * 
 * @param arr Array to check.
 * 
 * @return uint64 Number of elements.
 */
template<typename T>
inline uint64 size(const DynamicArray<T> &arr)
{
    return arr.num_elem;
}

/**
 * @brief Returns the current capacity of the Array.
 * 
 * @param arr Array to check.
 * 
 * @return uint64 Capacity of the Array.
 */
template<typename T>
inline uint64 capacity(const DynamicArray<T> &arr)
{
    return arr.cap;
}


// Iterator Methods ------------------------------------------------------------

/**
 * @brief Returns an Iterator to the first position in the Array.
 * 
 * @param arr Array to get iterator of.
 *
 * @return Iterator<T> Iterator to first position. 
 */
template<typename T>
inline Iterator<T> begin(DynamicArray<T> &arr)
{
    return Iterator<T>{arr.data};
}

/**
 * @brief Returns a ConstIterator to the first position in the Array.
 * 
 * @param arr Array to get iterator of.
 *
 * @return ConstIterator<T> Iterator to first position. 
 */
template<typename T>
inline ConstIterator<T> begin(const DynamicArray<T> &arr)
{
    return ConstIterator<T>{arr.data};
}

/**
 * @brief Returns an Iterator to one past the last element in the Array.
 *
 * @attention The returned iterator must not be dereferenced.
 * 
 * @param arr Array to get iterator of.
 *
 * @return Iterator<T> Iterator to one past the last element. 
 */
template<typename T>
inline Iterator<T> end(DynamicArray<T> &arr)
{
    return Iterator<T>{arr.data};
}

/**
 * @brief Returns a ConstIterator to one past the last element in the Array.
 * 
 * @attention The returned iterator must not be dereferenced.
 *
 * @param arr Array to get iterator of.
 *
 * @return ConstIterator<T> Iterator to one past the last element. 
 */
template<typename T>
inline ConstIterator<T> end(const DynamicArray<T> &arr)
{
    return ConstIterator<T>{arr.data};
}

/**
 * @brief Returns true if `first` and `second` point to the same address.
 *
 * Does not check if the iterators come from the same Array.
 * 
 * @param first First iterator. 
 * @param second Second iterator.
 *
 * @return bool True if iterators point to the same address. 
 */
template<typename T>
inline bool operator==(const Iterator<T> &first, const Iterator<T> &second)
{
    return first.datum == second.datum;
}

/**
 * @brief Returns true if `first` and `second` point to the same address.
 *
 * Does not check if the iterators come from the same Array.
 * 
 * @param first First iterator. 
 * @param second Second iterator.
 *
 * @return bool True if iterators point to the same address. 
 */
template<typename T>
inline bool operator==(const ConstIterator<T> &first, 
    const ConstIterator<T> &second)
{
    return first.datum == second.datum;
}

/**
 * @brief Increments the iterator by one position in the Array. Does not perform
 * bounds checking, simply increments the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return Iterator<T>& Reference to the iterator passed in.
 */
template<typename T>
inline Iterator<T>& operator++(Iterator<T> &it)
{
    ++it.datum;
    return it;
}

/**
 * @brief Increments the iterator by one position in the Array. Does not perform
 * bounds checking, simply increments the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return ConstIterator<T>& Reference to the iterator passed in.
 */
template<typename T>
inline ConstIterator<T>& operator++(ConstIterator<T> &it)
{
    ++it.datum;
    return it;
}

/**
 * @brief Increments the iterator by one position in the Array, returning a copy
 * of the iterator before it was incremented. Does not perform bounds checking, 
 * simply increments the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return Iterator<T> Copy of the iterator before incrementing.
 */
template<typename T>
inline Iterator<T> operator++(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    ++it;
    return copy;
}

/**
 * @brief Increments the iterator by one position in the Array, returning a copy
 * of the iterator before it was incremented. Does not perform bounds checking, 
 * simply increments the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return ConstIterator<T> Copy of the iterator before incrementing.
 */
template<typename T>
inline ConstIterator<T> operator++(ConstIterator<T> &it, int /** postfix */)
{
    ConstIterator<T> copy = it;
    ++it;
    return copy;
}

/**
 * @brief Decrements the iterator by one position in the Array. Does no perform
 * bounds checking, simply decrements the pointed to address.
 * 
 * @param it Iterator to decrement.
 * 
 * @return Iterator<T>& Reference to the iterator passed in. 
 */
template<typename T>
inline Iterator<T>& operator--(Iterator<T> &it)
{
    --it.datum;
    return it;
} 

/**
 * @brief Decrements the iterator by one position in the Array. Does no perform
 * bounds checking, simply decrements the pointed to address.
 * 
 * @param it Iterator to decrement.
 * 
 * @return ConstIterator<T>& Reference to the iterator passed in. 
 */
template<typename T>
inline ConstIterator<T>& operator--(ConstIterator<T> &it)
{
    --it.datum;
    return it;
}

/**
 * @brief Decrements the iterator by one position in the Array, returning a copy
 * of the iterator before it was incremented. Does not perform bounds checking, 
 * simply decrements the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return Iterator<T> Copy of the iterator before decrementing.
 */
template<typename T>
inline Iterator<T> operator--(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    --it;
    return copy;
}

/**
 * @brief Decrements the iterator by one position in the Array, returning a copy
 * of the iterator before it was incremented. Does not perform bounds checking, 
 * simply decrements the pointed to address.
 * 
 * @param it Iterator to increment.
 * 
 * @return ConstIterator<T> Copy of the iterator before decrementing.
 */
template<typename T>
inline ConstIterator<T> operator--(ConstIterator<T> &it, int /** postfix */)
{
    ConstIterator<T> copy = it;
    --it;
    return copy;
}

/**
 * @brief Returns a reference to the value the Iterator is pointing to. Does not
 * ensure the iterator is pointing to a valid Array element.
 * 
 * @param it Array to dereference.
 * 
 * @return T& Reference to the element pointed to by the iterator. 
 */
template<typename T>
inline T& operator*(const Iterator<T> &it)
{
    return *it.datum;
}

/**
 * @brief Returns a const-reference to the value the Iterator is pointing to. 
 * Does not ensure the iterator is pointing to a valid Array element.
 * 
 * @param it Array to dereference.
 * 
 * @return const T& Const-reference to the element pointed to by the iterator. 
 */
template<typename T>
inline const T& operator*(const ConstIterator<T> &it)
{
    return *it.datum;
}

} // namespace Tundra::DynArr