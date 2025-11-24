/**
 * @file LinkedList.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing
 * elements.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/containers/DynamicStack.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"


namespace Tundra::LnkLst
{

namespace Internal
{

// Definitions -----------------------------------------------------------------

// Default capacity in elements of a List.
constexpr uint64 DEF_CAP = 4;

// Index into a List's Node array that points to the sentinel of the List, which
// represents the end of the List.
constexpr uint64 SENTINEL_IDX = 0;


// Private Containers ----------------------------------------------------------

/**
 * @brief Represents a single element in the LinkedList.
 *
 * Linked together with other Nodes where each stores the index of the next and 
 * previous Node. Does not track the List it belongs to.
 * 
 * @tparam T Element type.
 */
template<typename T>
struct Node
{
    // datum.
    T datum;

    // Index of the next node, or SENTINEL_IDX if there isn't one.
    uint64 next;

    // Index of the previous node, or SENTINEL_IDX if there isn't one.
    uint64 prev;
};

} // namespace Internal


// Public Containers -----------------------------------------------------------

/**
 * @brief Dynamically linked container for efficiently inserting and removing
 * elements.
 * 
 * @tparam T Element type. 
 */
template<typename T>
struct LinkedList
{
    // Array of nodes.
    Internal::Node<T> *nodes;

    // Index of the head Node, or SENTINEL_IDX if there isn't one.
    uint64 head_idx;
    
    // Index of the tail Node, or SENTINEL_IDX if there isn't one.
    uint64 tail_idx;

    // Number of Nodes in the List.
    uint64 num_node;

    // Current maximum number of elements that can be added to the List before
    // it needs to be resized.
    uint64 cap;

    // Indexes inside the underlying array of Nodes that have been freed and 
    // can be reused.
    DynStk::DynamicStack<uint64> freed_idx;
};

/**
 * @brief Iterator for LinkedList.
 *
 * Provides pointer-like operations (dereference, increment/decrement, 
 * comparisons) for traversing and accessing elements.
 * 
 * @tparam T Element type.
 */
template<typename T>
struct Iterator
{
    // Reference to the List that owns this iterator.
    const LinkedList<T> &owner;

    // Index into the `owner` List where this iterator is pointing.
    uint64 idx_of_datum;
};

/**
 * @brief Const iterator for LinkedList.
 *
 * Provides pointer-like operations (dereference, increment/decrement, 
 * comparisons) for traversing and accessing elements.
 * 
 * @tparam T Element type.
 */
template<typename T>
struct ConstIterator
{
    // Reference to the List that owns this iterator.
    const LinkedList<T> &owner;

    // Index into the `owner` List where this iterator is pointing.
    uint64 idx_of_datum;
};


// Internal Methods ------------------------------------------------------------

namespace Internal
{

template<typename T>
inline void update_tail_idx(LinkedList<T> &list, uint64 idx)
{
    list.tail_idx = idx; // Update tail idx.
    list.nodes[SENTINEL_IDX].prev = idx; // Update sentinel prev value.
}

template<typename T>
inline void internal_init(LinkedList<T> &list, uint64 init_cap)
{
    DynStk::init(list.freed_idx);

    // Allocate Nodes array
    list.nodes = static_cast<Internal::Node<T>*>(
        alloc_mem(init_cap * sizeof(Internal::Node<T>*)));
    
    // Initialize Sentinel.
    list.nodes[SENTINEL_IDX].next = SENTINEL_IDX; // Points to itself, no next.
    // `prev` member of the sentinel is set with the `update_tail_idx` call.

    list.head_idx = SENTINEL_IDX;
    update_tail_idx(list, SENTINEL_IDX);

    list.num_node = 0;
    list.cap = init_cap;
}

} // namespace Internal


/**
 * @brief Initializes a List with default capacity. Allocates memory and sets
 * internal components.
 * 
 * @param list List to init. 
 */
template<typename T>
inline void init(LinkedList<T> &list)
{
    Internal::internal_init(list, Internal::DEF_CAP);
}

/**
 * @brief Initializes a List with a set capacity. Allocates memory and sets 
 * internal components.
 * 
 * @param list List to init.
 * @param init_cap Specified initial capacity.
 */
template<typename T>
inline void init(LinkedList<T> &list, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? Internal::DEF_CAP : init_cap;

    Internal::internal_init(list, init_cap);
}

/**
 * @brief Initializes a List with initial elements. Allocates memory and sets
 * internal components.
 *
 * `elements` are copied into the List. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if
 * exactly enough memory for `num_elem` should be allocated for the List. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will be
 * allocated to optimize against immediate reallocation on the next add request.
 * 
 * A fatal is thrown if `num_elem` is 0 with the List left uninitialized.
 *
 * @param list List to init.
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
template<typename T>
inline void init(LinkedList<T> &list, const T *elements, uint64 num_elem,
    bool strict_alloc = false)
{
    if(num_elem == 0)
    {
        TUNDRA_FATAL("Attempted to initialize list but initial number of "
            "elements is 0.");
        return;
    }

    // Number of elements needed for allocating with an extra space in mind for
    // the sentinel.
    const uint64 NUM_ELEM_W_SENT = num_elem + 1;
    const uint64 NUM_RESERVE_BYTE = 
            NUM_ELEM_W_SENT * sizeof(Internal::Node<T>);

    free_mem(list.nodes);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        // Allocate Nodes array.
        list.nodes = static_cast<Internal::Node<T>*>(
            alloc_mem(NUM_RESERVE_BYTE));
        list.cap = NUM_ELEM_W_SENT;
    }
    // Use the "reserving method" to alloc.
    else
    {
        // Temp var for retrieving the capacity of the allocated block through 
        // the reserve call. Capacity in bytes, not elements.
        Tundra::uint64 temp_cap_bytes;

        alloc_reserve_mem(
            reinterpret_cast<void**>(&list.nodes),
            &temp_cap_bytes,
            NUM_RESERVE_BYTE);
        list.cap = temp_cap_bytes / sizeof(Internal::Node<T>);
    }

    // Index to place the first Node at. Starts at 1 since the sentinel is 
    // at index 0,
    static constexpr uint64 FIRST_NODE_IDX = 1;

    // -- Initialize sentinel -- 

    // Sentinel points next to itself, no next.
    list.nodes[Internal::SENTINEL_IDX].next = Internal::SENTINEL_IDX;
    // `prev` member is set with the `update_tail_idx` call below.

    // -- Place first Node -- 

    // Place the first Node since it doesn't need to reset the `next` member of 
    // the Node before it.
    list.nodes[FIRST_NODE_IDX].datum = elements[0];
    list.nodes[FIRST_NODE_IDX].prev = Internal::SENTINEL_IDX;

    // -- Place the rest of the Nodes --

    // Start at 1 past the first Node index since we've added the initial Node.
    for(uint64 i = FIRST_NODE_IDX + 1; i < num_elem + 1; ++i)
    {
        list.nodes[i - 1].next = i;
        list.nodes[i].prev = i - 1;
        list.nodes[i].datum = elements[i - 1];
    }

    // Set the last added Node's next value since it does not have a next Node.
    // Use `num_elem` straight here since we're ahead by 1 in the List compared
    // to the initial elements array due to the sentinel.
    list.nodes[num_elem].next = Internal::SENTINEL_IDX;

    // -- Set container components --
    list.head_idx = FIRST_NODE_IDX;
    Internal::update_tail_idx(list, num_elem);
    list.num_node = num_elem;
}

/**
 * @brief Frees memory allocated for a List.
 * 
 * After calling this method, the List must not be used unless reinitialized.
 *
 * It is safe to call this method on a List that has already been freed, or 
 * never initialized.
 * 
 * @param list List to free.
 */
template<typename T>
inline void free(LinkedList<T> &list)
{
    DynStk::free(list.freed_idx);
    free_mem(list.nodes);
    list.nodes = nullptr;
}

/**
 * @brief Deep copies `src` to `dst`.
 *
 * If the Lists are of the same address, nothing is done.
 * 
 * `dst` can be an uninitialized List.
 *
 * @param src List to source from. 
 * @param dst List to deep copy to, can be uninitialized.
 */
template<typename T>
inline void copy(const LinkedList<T> &src, LinkedList<T> &dst)
{
    if(&dst == &src) { return; }

    const uint64 SRC_CAP_BYTE = src.cap * sizeof(Internal::Node<T>);

    if(dst.cap != src.cap || dst.nodes == nullptr)
    {
        free_mem(dst.nodes);
        dst.nodes = static_cast<Internal::Node<T>*>(alloc_mem(SRC_CAP_BYTE));
        dst.cap = src.cap;
    }

    copy_mem_fwd(src.nodes, dst.nodes, SRC_CAP_BYTE);
    dst.num_node = src.num_node;
    Tundra::DynStk::copy(src.freed_idx, dst.freed_idx);
    dst.head_idx = src.head_idx;
    dst.tail_idx = src.tail_idx;
}


} // namespace Tundra::LnkLst
