/**
 * @file LinkedList.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting, removing and
 *    traversing elements.
 * @version 0.1
 * @date 07-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/containers/DynamicArray.hpp"
#include "tundra/utils/containers/DynamicStack.hpp"


namespace Tundra::LnkLst
{

namespace Internal
{

// Default memory alignment.
static constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

// Default capacity in elements of an Array.
static constexpr Tundra::uint8 DEFAULT_CAPACITY = 4;


template<typename T>
struct Node
{
    T value;

    // Index inside the List's array of the next Node in the List.
    Tundra::int64 next;
};

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Dynamically linked container for efficiently inserting, removing and
 * traversing elements.
 * 
 * @tparam T Type the List stores.
 * @tparam alignment Alignment in bytes to align the List's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<typename T, 
    Tundra::uint8 alignment = Tundra::LnkLst::Internal::DEFAULT_ALIGNMENT>
struct LinkedList 
{
    // Array of Nodes.
    Tundra::DynArr::DynamicArray<
        Tundra::LnkLst::Internal::Node<T>, alignment> nodes;

    // Index of the head node in the Array.
    Tundra::uint64 head_index;

    // Number of Nodes in the List.
    Tundra::uint64 num_nodes;

    // Indexs inside the Array that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::uint64, alignment> freed_indexes;
};


// Internal Methods ---------------------------------------------------------------

namespace Internal
{

} // namespace Internal


// Public ---------------------------------------------------------------------

} // namespace Tundra::LnkLst
