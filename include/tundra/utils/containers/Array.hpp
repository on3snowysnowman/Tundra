/**
 * @file Array.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed sized contiguous Array for storing elements.
 * @date 2025-10-13
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/FatalHandler.hpp"


namespace Tundra::Arr
{

namespace Internal
{

// Default cap in elements of an Array.
constexpr Tundra::uint64 DEFAULT_CAPACITY = 4;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Fixed size contiguous container for storing elements.
 * 
 * @tparam T Type the Array stores.
 * @tparam cap Fixed size of the Array.
 */
template<typename T, 
    Tundra::uint64 cap = Internal::DEFAULT_CAPACITY>
struct Array
{
    static_assert(cap > 0, "Capacity must be greater than 0.");

    T data[cap];
};

/**
 * @brief Container for storing a pointer to a single element in the Array.
 *
 * Used for iterating over the Array's elements.
 * 
 * @tparam T Type the Iterator tracks.
 */
template<typename T>
struct Iterator
{
    T *data;
};

/**
 * @brief Container for storing a const-pointer to a single element in the 
 * Array.
 *
 * Used for iterating over the Array's elements with const protection.
 * 
 * @tparam T Type the Iterator tracks.
 */
template<typename T>
struct ConstIterator
{
    const T *data;
};


// Public ----------------------------------------------------------------------

/**
 * @brief Creates an Array with an initial list of elements.
 * 
 * @note Consteval function, creation happens during compile time, similar to 
 * creating a default initialized stack allocated C array.
 *
 * @param args Elements to fill Array with.
 *
 * @return consteval Array<T, sizeof...(Ts)> Created Array with 
 *    initial elements.
 */
template<typename T, typename... Ts>
consteval Array<T, sizeof...(Ts)> make(Ts&&... args)
{
    return Array<T, sizeof...(Ts)>{ { T{args}... } };
}

/**
 * @brief Returns a reference to the value at an index with index checking.
 *
 * Performs a bounds check on `index`. If `index` is out of range, calls the
 * configured fatal handler. If the handler returns, behavior is undefined.
 * 
 * @param arr Reference to the Array. 
 * @param index Index.
 *
 * @return T& Reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
inline T& at(Array<T, cap> &arr, Tundra::uint64 index)
{
    // If the index is invalid.
    if(index >= cap)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            cap);
    }

    return arr.data[index];
}

/**
 * @brief Returns a const-reference to the value at an index with index 
 * checking.
 *
 * Performs a bounds check on `index`. If `index` is out of range, calls the
 * configured fatal handler. If the handler returns, behavior is undefined.
 * 
 * @param arr Const-reference to the Array. 
 * @param index Index.
 *
 * @return T& Const-reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
inline const T& at(const Array<T, cap> &arr, Tundra::uint64 index)
{
    // If the index is invalid.
    if(index >= cap)
    {
        TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
            cap);
    }

    return arr.data[index];
}

/**
 * @brief Returns a reference to the value at an index without index checking.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid or 
 * undefined behavior will occur.
 *
 * @param arr Reference to the Array. 
 * @param index Index.
 *
 * @return T& Reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
constexpr T& at_unchecked(Array<T, cap> &arr, 
    Tundra::uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a const-reference to the value at an index without index 
 * checking.
 * 
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid or 
 * undefined behavior will occur.
 *
 * @param arr Const-reference to the Array. 
 * @param index Index.
 *
 * @return T& Const-reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
constexpr const T& at_unchecked(const Array<T, cap> &arr, 
    Tundra::uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a reference to the element at the front of the Array.
 * 
 * @param arr Reference to the Array.
 * 
 * @return T& Reference to front element. 
 */
template<typename T, Tundra::uint64 cap>
constexpr T& front(Array<T, cap> &arr)
{
    return arr.data[0];
}


/**
 * @brief Returns a const-reference to the element at the front of the Array.
 * 
 * @param arr Const-reference to the Array.
 * 
 * @return T& Const-reference to the front element. 
 */
template<typename T, Tundra::uint64 cap>
constexpr const T& front(const Array<T, cap> &arr)
{
    return arr.data[0];
}

/**
 * @brief Returns a reference to the element at the back of the Array.
 * 
 * @param arr Reference to the Array.
 * 
 * @return T& Reference to back element. 
 */
template<typename T, Tundra::uint64 cap>
constexpr T& back(Array<T, cap> &arr)
{
    return arr.data[cap - 1];
}

/**
 * @brief Returns a const-reference to the element at the front of the Array.
 * 
 * @param arr Const-reference to the Array.
 * 
 * @return T& Reference to back element. 
 */
template<typename T, Tundra::uint64 cap>
constexpr const T& back(const Array<T, cap> &arr)
{
    return arr.data[cap - 1];
}

/**
 * @brief Returns the fixed size of the Array.
 * 
 * @note Consteval method.
 *
 * @param arr Optional Array parameter for auto template deduction.
 *
 * @return Tundra::uint64 Capacity.
 */
template<typename T, Tundra::uint64 cap>
constexpr Tundra::uint64 size(const Array<T, cap>& /** &arr */)
{
    return cap;
}


// Iterator Methods ------------------------------------------------------------

/**
 * @brief Returns an Iterator to the first element in the Array.
 * 
 * @param arr Reference to the Array. 
 * @return Iterator<T> Iterator to the first element.
 */
template<typename T, Tundra::uint64 cap>
constexpr Iterator<T> begin(Array<T, cap> &arr)
{
    return Iterator<T>{arr.data};
}

/**
 * @brief Returns a ConstIterator to the first element in the Array.
 * 
 * @param arr Const-reference to the Array. 
 * @return ConstIterator<T> Const iterator to the first element.
 */
template<typename T, Tundra::uint64 cap>
constexpr ConstIterator<T> begin(const Array<T, cap> &arr)
{
    return ConstIterator<T>{arr.data};
}

/**
 * @brief Returns an Iterator to one past the last element in the Array.
 * 
 * The returned iterator must not be dereferenced.
 *
 * @param arr Reference to the Array. 
 * @return Iterator<T> Iterator to the element one past the end..
 */
template<typename T, Tundra::uint64 cap>
constexpr Iterator<T> end(Array<T, cap> &arr)
{
    return Iterator<T>{arr.data + cap};
}

/**
 * @brief Returns a ConstIterator to one past the last element in the Array.
 * 
 * The returned iterator must not be dereferenced.
 *
 * @param arr Const-reference to the Array. 
 * @return ConstIterator<T> Const iterator to the element one past the end..
 */
template<typename T, Tundra::uint64 cap>
constexpr ConstIterator<T> end(const Array<T, cap> &arr)
{
    return ConstIterator<T>{arr.data + cap};
}

/**
 * @brief Returns true if the address the two Iterators point to are the same.
 *
 * Does not check whether the iterators come from the same Array instance. 
 * 
 * @param first Const-reference to the first iterator. 
 * @param second Const-reference to the second iterator.
 * @return bool True if the iterators point to the same address.
 */
template<typename T>
constexpr bool operator==(const Iterator<T> &first, const Iterator<T> &second)
{
    return first.data == second.data;
}

/**
 * @brief Returns true if the address the two ConstIterators point to are the 
 * same.
 *
 * Does not check whether the iterators come from the same Array instance. 
 * 
 * @param first Const-reference to the first iterator. 
 * @param second Const-reference to the second iterator.
 * @return bool True if the iterators point to the same address.
 */
template<typename T>
constexpr bool operator==(const ConstIterator<T> &first, 
    const ConstIterator<T> &second)
{
    return first.data == second.data;
}

/**
 * @brief Increments an Iterator to the next element in the Array, returning the
 * incremented iterator.
 * 
 * @param it Reference to the iterator. 
 * @return Iterator<T>& Reference to the iterator passed in.
 */
template<typename T>
constexpr Iterator<T>& operator++(Iterator<T> &it)
{
    ++it.data;
    return it;
}

/**
 * @brief Increments a ConstIterator to the next element in the Array, returning
 * the incremented iterator.
 * 
 * @param it Reference to the iterator. 
 * @return ConstIterator<T>& Reference to the iterator passed in.
 */
template<typename T>
constexpr ConstIterator<T>& operator++(ConstIterator<T> &it)
{
    ++it.data;
    return it;
}

/**
 * @brief Increments an Iterator to the next element in the Array, returning a
 * copy of the iterator before it was incremented.
 *  
 * @param it Reference to the iterator.
 * @return Iterator<T> Copy of the iterator before incrementing.
 */
template<typename T>
constexpr Iterator<T> operator++(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    ++it;
    return copy;
}

/**
 * @brief Increments a ConstIterator to the next element in the Array, returning 
 * a copy of the iterator before it was incremented.
 *  
 * @param it Reference to the iterator.
 * @return ConstIterator<T> Copy of the iterator before incrementing.
 */
template<typename T>
constexpr ConstIterator<T> operator++(ConstIterator<T> &it, int /** postfix */)
{
    ConstIterator<T> copy = it;
    ++it;
    return copy;
}

/**
 * @brief Decrements an Iterator to the previous element in the Array, 
 * returning the decremented iterator.
 * 
 * @param it Reference to the iterator. 
 * @return Iterator<T>& Reference to the iterator passed in.
 */
template<typename T>
constexpr Iterator<T>& operator--(Iterator<T> &it)
{
    --it.data;
    return it;
}

/**
 * @brief Decrements a ConstIterator to the previous element in the Array, 
 * returning the decremented iterator.
 * 
 * @param it Reference to the iterator. 
 * @return ConstIterator<T>& Reference to the iterator passed in.
 */
template<typename T>
constexpr ConstIterator<T>& operator--(ConstIterator<T> &it)
{
    --it.data;
    return it;
}

/**
 * @brief Decrements an Iterator to the previous element in the Array, returning 
 * a copy of the iterator before it was decremented.
 *  
 * @param it Reference to the iterator.
 * @return Iterator<T> Copy of the iterator before decrementing.
 */
template<typename T>
constexpr Iterator<T> operator--(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    --it;
    return copy;
}

/**
 * @brief Decrements a ConstIterator to the previous element in the Array, 
 * returning  a copy of the iterator before it was decremented.
 *  
 * @param it Reference to the iterator.
 * @return ConstIterator<T> Copy of the iterator before decrementing.
 */
template<typename T>
constexpr ConstIterator<T> operator--(ConstIterator<T> &it, int /** postfix */)
{
    ConstIterator<T> copy = it;
    --it;
    return copy;
}

/**
 * @brief Dereferences an Iterator, returning a reference to the value it is 
 * pointing to.
 * 
 * @param it Reference to the iterator. 
 * @return T& Reference to the data pointed to by the iterator.
 */
template<typename T>
constexpr T& operator*(const Iterator<T> &it)
{
    return *it.data;
}

/**
 * @brief Dereferences a ConstIterator, returning a const-reference to the value 
 * it is pointing to.
 * 
 * @param it Const-reference to the iterator. 
 * @return const T& Const-reference to the data pointed to by the iterator.
 */
template<typename T>
constexpr const T& operator*(const ConstIterator<T> &it)
{
    return *it.data;
}

} // namespace Tundra::Arr