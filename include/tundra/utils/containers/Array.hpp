/**
 * @file Array.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Fixed sized contiguous Array for storing elements.
 * @version 0.1
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
    Tundra::uint64 cap = Tundra::Arr::Internal::DEFAULT_CAPACITY>
struct Array
{
    T data[cap];
};

template<typename T>
struct Iterator
{
    T *data;
};


// Public ----------------------------------------------------------------------

/**
 * @brief Creates an Array with an initial list of elements.
 * 
 * @param args Elements to fill Array with.
 *
 * @return consteval Tundra::Arr::Array<T, sizeof...(Ts)> Created Array with 
 *    initial elements.
 */
template<typename T, typename... Ts>
consteval Tundra::Arr::Array<T, sizeof...(Ts)> make(Ts&&... args)
{
    return Tundra::Arr::Array<T, sizeof...(Ts)>{ { T{args}... } };
}

/**
 * @brief Returns a reference to the value at an index with index checking.
 *
 * Performs bounds check on `index`, terminating with a fatal error if it's 
 * invalid.
 * 
 * @param arr Reference to the Array. 
 * @param index Index.
 *
 * @return T& Reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
inline T& at(Tundra::Arr::Array<T, cap> &arr, Tundra::uint64 index)
{
    if(index < cap) { return arr.data[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but Array cap is: \"%llu\".", index, 
        cap);
}

/**
 * @brief Returns a const-reference to the value at an index with index 
 * checking.
 *
 * Performs bounds check on `index`, terminating with a fatal error if it's 
 * invalid.
 * 
 * @param arr Reference to the Array. 
 * @param index Index.
 *
 * @return T& Const-reference to the item at the index. 
 */
template<typename T, Tundra::uint64 cap>
inline const T& at(const Tundra::Arr::Array<T, cap> &arr, 
    Tundra::uint64 index)
{
    if(index < cap) { return arr.data[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but Array capacity is: \"%llu\".", index, 
        cap);
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
inline T& at_unchecked(Tundra::Arr::Array<T, cap> &arr, 
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
inline const T& at_unchecked(const Tundra::Arr::Array<T, cap> &arr, 
    Tundra::uint64 index)
{
    return arr.data[index];
}

/**
 * @brief Returns a pointer to the element at the front of the Array.
 * 
 * @param arr Reference to the Array.
 * 
 * @return T& Pointer to the item at the front. 
 */
template<typename T, Tundra::uint64 cap>
inline T& front(Tundra::Arr::Array<T, cap> &arr)
{
    return &arr.data[0];
}


/**
 * @brief Returns a const-pointer to the element at the front of the Array.
 * 
 * @param arr Const-reference to the Array.
 * 
 * @return T& Const-reference to the item at the front. 
 */
template<typename T, Tundra::uint64 cap>
inline const T& front(const Tundra::Arr::Array<T, cap> &arr)
{
    return arr.data[0];
}

/**
 * @brief Returns a pointer to the element at the back of the Array.
 * 
 * @param arr Reference to the Array.
 * 
 * @return T& Pointer to the item at the back. 
 */
template<typename T, Tundra::uint64 cap>
inline T& back(Tundra::Arr::Array<T, cap> &arr)
{
    return arr.data[cap - 1];
}

/**
 * @brief Returns a const-pointer to the element at the front of the Array.
 * 
 * @param arr Const-reference to the Array.
 * 
 * @return T& Const-pointer to the item at the front. 
 */
template<typename T, Tundra::uint64 cap>
inline const T& back(const Tundra::Arr::Array<T, cap> &arr)
{
    return arr.data[cap - 1];
}

/**
 * @brief Returns the fixed size of the Array.
 * 
 * @param arr Const-reference to the Array. 
 *
 * @return Tundra::uint64 Capacity.
 */
template<typename T, Tundra::uint64 cap>
consteval Tundra::uint64 size(const Tundra::Arr::Array<T, cap> &arr)
{
    return cap;
}


// Iterator Methods ------------------------------------------------------------

template<typename T, Tundra::uint64 cap>
inline Iterator<T> begin(Tundra::Arr::Array<T, cap> &arr)
{
    return Iterator<T>{arr.data};
}

template<typename T, Tundra::uint64 cap>
inline Iterator<T> end(Tundra::Arr::Array<T, cap> &arr)
{
    return Iterator<T>{arr.data + cap};
}

template<typename T>
inline bool operator==(const Iterator<T> &first, const Iterator<T> &second)
{
    return first.data == second.data;
}

template<typename T>
inline Iterator<T>& operator++(Iterator<T> &it)
{
    ++it.data;
    return it;
}

template<typename T>
inline Iterator<T> operator++(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    ++it;
    return copy;
}

template<typename T>
inline Iterator<T>& operator--(Iterator<T> &it)
{
    --it.data;
    return it;
}

template<typename T>
inline Iterator<T> operator--(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy = it;
    --it;
    return copy;
}

template<typename T>
inline T& operator*(Iterator<T> &it)
{
    return *it.data;
}

} // namespace Tundra::Arr