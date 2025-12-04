/**
 * @file LinkedList.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing 
 *    elements.
 * @version 0.1
 * @date 08-13-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/containers/DynamicStack.hpp"
#include "tundra/utils/FatalHandler.hpp"

namespace Tundra::LnkLst
{

namespace Internal
{

constexpr Tundra::uint8 DEFAULT_CAPACITY = 4;

// Index into a List's Node array that points to the sentinel of the List, which
// represents the end of the List. 
constexpr Tundra::int64 SENTINEL_IDX = 0;

/**
 * @brief Used in the LinkedList to represent a single value in the List. 
 *
 * Is linked together with other Nodes to create a linked list, where each Node 
 * points to the index of the next Node inside the List's array.
 * 
 * @tparam T Type the Node stores.
 */
template<typename T> 
struct Node
{
    // Value the Node stores.
    T value;

    // Index into the List's array of the next Node in the List. If there is no
    // next Node, this value will be `Internal::NO_NODE`.
    Tundra::int64 next;

    // Index into the List's array of the previous Node in the List. If there is
    // no previous Node, this value will be `Internal::NO_NODE`.
    Tundra::int64 previous;
}; // struct Node

}; // namespace Internal

/**
 * @brief Dynamically linked container for efficiently inserting and removing 
 *    elements.
 * 
 * @tparam T Type the List stores.
 */
template<typename T>
struct LinkedList 
{
    // Array of Nodes. 
    Internal::Node<T> *nodes;

    // Index of the head Node.
    Tundra::uint64 head_index;

    // Index of the tail Node.
    Tundra::uint64 tail_index;

    // Number of Nodes in the List.
    Tundra::uint64 num_nodes;

    // Current maximum number of elements that the List can store before it 
    // needs to be resized.
    Tundra::uint64 capacity;

    // Indexes inside the Array that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::uint64> freed_indexes;
};

template<typename T>
struct Iterator
{
    const LinkedList<T> &list_ref;
    Tundra::uint64 node_idx;
};


// Internal Methods ---------------------------------------------------------------

namespace Internal
{

/**
 * @brief Updates the tail index of the List and the sentintel's previous value
 * with a new Index.
 * 
 * @param list Reference to the List. 
 * @param new_idx New index to set.
 */
template<typename T>
inline void update_tail_idx(LinkedList<T> &list, Tundra::uint64 new_idx)
{
    list.tail_index = new_idx;
    list.nodes[SENTINEL_IDX].previous = new_idx;
}

/**
 * @brief Underlying initialization method. Allocates initial memory for 
 * `init_capacity` elements and sets internal values.
 * 
 * @param list Pointer to the List.
 * @param init_capacity Initial capacity in elements to allocate.
 */
template<typename T>
inline bool underlying_init(LinkedList<T> &list, Tundra::uint64 init_capacity)
{
    Tundra::DynStk::init(list.freed_indexes);

    // Allocate Nodes array.
    list.nodes = reinterpret_cast<Internal::Node<T>*>(
        Tundra::alloc_mem(init_capacity * sizeof(Internal::Node<T>)));
    if(list.nodes == nullptr) { return false; }

    // Initialize Sentinel.
    list.nodes[SENTINEL_IDX].next = SENTINEL_IDX; // Points to itself, no next.
    // Previous member is set with the `update_tail_idx` call below.

    list.head_index = SENTINEL_IDX;
    update_tail_idx(list, SENTINEL_IDX);

    list.num_nodes = 0;
    list.capacity = init_capacity;
    return true;
}   

/**
 * @brief Checks if the List has filled its allocated capacity, resizing if it
 * has.
 *  
 * @param list Pointer to the List.
 */
template<typename T>
inline bool check_and_handle_resize(LinkedList<T> &list)
{
    // Add one for sentinel
    if(list.num_nodes + 1 < list.capacity) { return true; }

    const Tundra::uint64 NEW_CAPACITY = 2 * list.capacity;

    // Get a new memory block that is twice the capacity of the current one and
    // copy over the old bytes.
    Internal::Node<T> *new_mem =
        reinterpret_cast<Internal::Node<T>*>
    ( 
        Tundra::alloc_copy_mem(
            list.nodes,
            NEW_CAPACITY * sizeof(Internal::Node<T>),
            list.num_nodes * sizeof(Internal::Node<T>))
    );
    
    if(new_mem == nullptr) { return false; }
    
    Tundra::free_mem(list.nodes);
    list.nodes = new_mem;
    list.capacity = NEW_CAPACITY;
    return true;
}

/**
 * @brief Finds the first available index that a Node can be placed at.
 *
 * Does not perform a range check on the List, assumes that there is space at 
 * the end for a new Node.
 * 
 * @param list Pointer to the list. 
 * @return Tundra::int64 Available index in the List.
 */
template<typename T>
inline Tundra::int64 get_available_index(LinkedList<T> &list)
{
    // If there are no available indexes that have been freed.
    if(Tundra::DynStk::is_empty(list.freed_indexes))
    {
        // Since there are no freed positions inside the array of Nodes, the 
        // first available position will always be at the end of the filled 
        // array of Nodes.
        return list.num_nodes;
    }

    // -- Fetch an available index from the stack of freed indexes. --

    Tundra::int64 avail_index =
        Tundra::DynStk::front(list.freed_indexes);
    Tundra::DynStk::pop(list.freed_indexes);
    return avail_index;
}

template<typename T>
inline Internal::Node<T>* find_from_start(LinkedList<T> &list, 
    Tundra::uint64 index) 
{
    Internal::Node<T> *parsed_node = &list.nodes[list.head_index];

    // Iterate through each Node in the List until we've reached the index.
    // Start at iteration 1 since we've already parsed the head Node.
    for(Tundra::uint64 i = 1; i <= index; ++i)
    {
        parsed_node = &list.nodes[parsed_node->next];
    }

    return parsed_node;
}

template<typename T>
inline Internal::Node<T>* find_from_end(LinkedList<T> &list, 
    Tundra::uint64 index) 
{
    Internal::Node<T> *parsed_node = &list.nodes[list.tail_index];

    // Since `index` gives us a parse amount from the start of the list, we 
    // need to calculate the index to stop at with respect to starting at the 
    // end of the List.
    const Tundra::uint64 INDEX_FROM_END = list.num_nodes - index;

    // Iterate backwards through each Node in the List until we've reached the
    // index. Start at iteration 1 since we've already parsed the tail Node.
    for(Tundra::uint64 i = 1; i < INDEX_FROM_END; i--)
    {
        parsed_node = &list.nodes[parsed_node->previous];
    }

    return parsed_node;
}

/**
 * @brief Returns a pointer to the Node at the index.
 * 
 * @param list Pointer to the List. 
 * @param index Index of the Node.
 *
 * @return Internal::Node<T>* Pointer to the Node at `index`.
 */
template<typename T>
inline Internal::Node<T>* get_node(LinkedList<T> &list, Tundra::uint64 index)
{
    // If the index is in the first half of the list, iterate from the start. 
    // Otherwise, iterate from the end.
    return (index < list.num_nodes / 2) ? 
        Internal::find_from_start(list, index) : 
        Internal::find_from_end(list, list.num_nodes - index - 1);
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a List with default capacity. Allocates memory and resets
 * internal components.
 * 
 * @param list Pointer to the List.
 */
template<typename T>
inline bool init(LinkedList<T> &list)
{
    return Internal::underlying_init(list, Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes a List with a specified capacity. Allocates memory and 
 * resets internal components.
 *
 * If `init_capacity` is 0, it is set to the default capacity.
 * 
 * @param list Pointer to the List. 
 * @param init_capacity Initial capacity in elements.
 */
template<typename T>
inline bool init(LinkedList<T> &list, Tundra::uint64 init_capacity)
{
    init_capacity = (init_capacity == 0) ? 
        Internal::DEFAULT_CAPACITY : init_capacity;

    return Internal::underlying_init(list, init_capacity);
}

/**
 * @brief Initializes a List with an initial array of elements. Allocates at 
 * least enough memory for `num_elements` and resets internal components.
 *
 * Elements are copied in from `init_elements`. The number of elements to copy 
 * must be and assumed to be greater than 0.
 * 
 * @param list Pointer to the List.
 * @param init_elements Read-only pointer to the array of initial elements.
 * @param num_elements Number of elements to copy.
 */
template<typename T>
inline bool init(LinkedList<T> &list, const T *init_elements, 
    Tundra::uint64 num_elements)
{
    // Increment num_elements by 1 to account for the required sentinel.
    ++num_elements;  

    Tundra::DynStk::init(list.freed_indexes);

    // New capacity in bytes of the List, will be set by the following 
    // reserve call.
    Tundra::uint64 new_capacity_bytes;

    const Tundra::uint64 NUM_RESERVE_BYTES = 
        num_elements * sizeof(Internal::Node<T>);

    Tundra::alloc_reserve_mem(&list.nodes,
        &new_capacity_bytes, NUM_RESERVE_BYTES);

    if(list.nodes == nullptr) { return false; }

    list.capacity = new_capacity_bytes / sizeof(T);
    
    // Index where to place the first Node in the Nodes array. Start at 1 since
    // the sentinel lives at the 0th position.
    static constexpr Tundra::uint64 FIRST_NODE_IDX = 1;

    // Initialize Sentinel.

    // Points to itself, no next.
    list.nodes[Internal::SENTINEL_IDX].next = Internal::SENTINEL_IDX; 
    // Previous member is set with the `update_tail_idx` call below.

    // Place the first Node since it doesn't need to reset the `next` member of 
    // the Node before it.
    list.nodes[FIRST_NODE_IDX].value = init_elements[0];
    list.nodes[FIRST_NODE_IDX].previous = Internal::SENTINEL_IDX;

    // Start at 1 past the first Node index since we've added the initial Node.
    Tundra::uint64 i = FIRST_NODE_IDX + 1;
    while(i < num_elements)
    {
        list.nodes[i - 1].next = i;
        list.nodes[i].previous = i - 1;
        list.nodes[i].value = init_elements[i];
        ++i;
    }

    // Set the last added Node's next value since it does not have a next Node.
    list.nodes[i - 1].next = Internal::SENTINEL_IDX;

    list.head_index = Internal::SENTINEL_IDX;
    Internal::update_tail_idx(list, Internal::SENTINEL_IDX);
    list.num_nodes = num_elements;
    return true;
}

/**
 * @brief Released heap memory allocated for the List.
 * 
 * @param list Pointer to the List. 
 */
template<typename T>
inline void free(LinkedList<T> &list)
{
    Tundra::DynStk::free(list.freed_indexes);

    Tundra::free_mem((void*)list.nodes);
    list.nodes = nullptr;
}

/**
 * @brief Resets the List to an empty state.
 *
 * This does not modify, shrink, deallocate, or zero out the underlying memory.
 * Only the internal tracking elements for the Nodes are reset, so subsequent 
 * adds will overwrite previous data.
 * 
 * @param list Pointer to the List.
 */
template<typename T>
inline void clear(LinkedList<T> &list)
{
    Tundra::DynStk::clear(list.freed_indexes);

    list.num_nodes = 0;
    list.head_index = Internal::SENTINEL_IDX;
    Internal::update_tail_idx(list, Internal::SENTINEL_IDX);
}

template<typename T>
inline bool add_front(LinkedList<T> &list, const T &element)
{
    if(list.num_nodes == 0)
    {
        // Place new Node at the first spot after the sentinel, since there are 
        // no other Nodes.
        static constexpr Tundra::int64 IDX = Internal::SENTINEL_IDX + 1;

        list.head_index = IDX;
        Internal::update_tail_idx(list, IDX);
        list.nodes[IDX].value = element;
        list.nodes[IDX].next = Internal::SENTINEL_IDX;
        list.nodes[IDX].previous = Internal::SENTINEL_IDX;
        ++list.num_nodes;
        return true;
    }

    if(!Internal::check_and_handle_resize(list))
    {
        return false;
    }

    Tundra::int64 available_index = 
        Internal::get_available_index(list);

    list.nodes[available_index].value = element;
    list.nodes[available_index].next = list.head_index;
    list.nodes[available_index].previous = Internal::SENTINEL_IDX;

    // Update the head node to point to this Node as previous.
    list.nodes[list.head_index].previous = available_index;

    // Update head index to the new position.
    list.head_index = available_index;

    ++list.num_nodes;
    return true;
}

/**
 * @brief Adds an element to the end of the List.
 * 
 * @param list Pointer to the List. 
 * @param element Read-only pointer to the element to add.
 */
template<typename T>
inline bool add_end(LinkedList<T> &list, const T &element)
{
    if(list.num_nodes == 0)
    {
        // Place new Node at the first spot after the sentinel, since there are 
        // no other Nodes.
        static constexpr Tundra::int64 IDX = Internal::SENTINEL_IDX + 1;

        list.head_index = IDX;
        Internal::update_tail_idx(list, IDX);
        list.nodes[IDX].value = element;
        list.nodes[IDX].next = Internal::SENTINEL_IDX;
        list.nodes[IDX].previous = Internal::SENTINEL_IDX;
        ++list.num_nodes;
        return true;
    }

    if(!Internal::check_and_handle_resize(list)) 
        { return false; }

    Tundra::uint64 available_index = 
        Tundra::LnkLst::Internal::get_available_index(list);

    list.nodes[available_index].value = element;
    list.nodes[available_index].next = Internal::SENTINEL_IDX;
    list.nodes[available_index].previous = list.tail_index;

    // Update the tail node to point to this Node as next.
    list.nodes[list.tail_index].next = available_index;

    // Update tail index to the new position.
    Internal::update_tail_idx(list, available_index);

    ++list.num_nodes;
    return true;
}

/**
 * @brief Inserts an element at the given index, returning true if successful. 
 *
 * Performs a bound check on the index.
 * 
 * @param list Pointer to the List.
 * @param element Read-only pointer to the element to insert.
 * @param index Index to insert.
 *
 * @return bool True if insertion was successful, false otherwise. 
 */
template<typename T>
inline bool insert(LinkedList<T> &list, const T &element, Tundra::uint64 index)
{
    if(index >= list.num_nodes) { return false; }

    // The index is the last position in the list.
    if(index == list.num_nodes - 1)
    {
        // Simply add to the end.
        Tundra::LnkLst::add_end(list, element);
        return true;
    }

    if(!Internal::check_and_handle_resize(list)) 
        { return false; }

    // The current Node at the index to insert at.
    Internal::Node<T> *node_at_insertion_index = 
        Internal::get_node(list, index);
    
    // Get the available index that the new Node will be placed at.
    Tundra::uint64 avail_index = 
        Internal::get_available_index(list);

    // The new Node being inserted in the List.
    Internal::Node<T> *new_node = &list.nodes[avail_index];

    // // Node that is behind in the chain of the target node we're inserting at.
    // Internal::Node<T> *behind_node = 
    //     &list.nodes[node_at_insertion_index->previous];

    // Node that is ahead in the chain of the target node we're inserting at. It
    // is known that there is another node in the list due to the check at the 
    // top of the method if the targ index position is the last Node in the 
    // List.
    Internal::Node<T> *ahead_node = 
        &list.nodes[node_at_insertion_index->next];

    // Update the new Node to point to the insertion index Node. We do this by
    // using the Node ahead of the insertion index Node's previous index, which
    // will point to where the insertion index Node is. 
    new_node->next = ahead_node->previous;
    
    // Update the new Node to point backwards to the Node that is behind the 
    // insertion index Node.
    new_node->previous = node_at_insertion_index->previous;

    // Update new Node's value.
    new_node->value = *element;

    // If we're not inserting at the head index, there is a Node previously 
    // behind the insertion index Node that needs to be updated. 
    if(index != list.head_index)
    {
        // Update the Node previously behind the insertion index Node to now 
        // point to the new Node.
        list.nodes[node_at_insertion_index->previous].next = avail_index;
    }

    // Update the insertion index Node to point backward to the new Node.
    node_at_insertion_index->previous = avail_index;

    ++list.num_nodes;

    return true;
}

/**
 * @brief Removes the element at the specified index, returning true if 
 * successful.
 *
 * Must perform a linear search through the list, either from the start or end,
 * to find the Node.
 * 
 * @param list Pointer to the List.
 * @param index Index to erase
 *
 * @return bool True if erasure was successful, false otherwise. 
 */
template<typename T>
inline bool erase(LinkedList<T> &list, Tundra::uint64 index)
{
    if(index >= list.num_nodes) { return false; }

    // If we are erasing the head Node.
    if(index == 0)
    {
        // Set the head index to the index that the head Node was pointing to.
        list.head_index = list.nodes[list.head_index].next;
        --list.num_nodes;
        return true;
    }

    // Previous Node points to the head Node initially, since we've already 
    // checked that we are not removing it.
    Internal::Node<T> *previous_node = list.nodes[list.head_index];
    Internal::Node<T> *parsed_node = list.nodes[previous_node->next];

    // Iterate until we've reached the target index.
    for(Tundra::uint64 i = 1; i < index; ++i)
    {
        previous_node = parsed_node;
        parsed_node = &list.nodes[parsed_node->next];
    }

    // -- `parsed_node` now points to the Node we want to remove. --

    // Add the index of the Node we are removing to the stack of freed indexes.
    Tundra::DynStk::push(&list.freed_indexes, previous_node->next);

    // Update the previous Node to point to what the parsed Node is pointing to,
    // since we're removing the parsed Node.
    previous_node->next = parsed_node->next;

    --list.num_nodes;
    return true;
}

/**
 * @brief Removes the element at the back of the List, returning true if 
 * successful.
 *
 * Performs a check if the List has a back element to remove.
 *
 * Using this method instead of the typical `erase` method skips the need to 
 * perform a full linear search to find the Node to erase, so it is much more 
 * efficient to use this method.
 * 
 * @param list Pointer to the List.
 * 
 * @return bool True if erasure was successful, false otherwise.
 */
template<typename T> 
inline bool erase_back(LinkedList<T> &list)
{
    if(!(bool)list.num_nodes) { return false; }

    // The List only contains the head Node, so no index pointers need to be 
    // updated, only removed.
    if(list.num_nodes == 1)
    {
        --list.num_nodes;
        // Head index and Tail index are already at 0.
        return true;
    }

    // Index of the Node that is second to the tail.
    Tundra::uint64 second_tail_index = 
        list.nodes[list.tail_index].previous;

    // Update the second to last node to point to the sentinel.
    list.nodes[second_tail_index].next = Internal::SENTINEL_IDX;

    Internal::update_tail_idx(list, second_tail_index);

    --list.num_nodes; 
    return true;
}

/**
 * @brief Returns a pointer to first Node in the List.
 * 
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 *
 * @param list Pointer to the List.
 *
 * @return T& Reference to the first element in the List.
 */
template<typename T>
inline T& front(LinkedList<T> &list)
{
    return list.nodes[list.head_index].value;
}

template<typename T>
inline const T& front(const LinkedList<T> &list)
{
    return list.nodes[list.head_index].value;
}

/**
 * @brief Returns a pointer to the last Node in the List.
 *
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list Pointer to the List.
 * 
 * @return Internal::Node<T>* Pointer to the last Node in the List. 
 */
template<typename T>
inline T& back(LinkedList<T> &list)
{
    return list.nodes[list.tail_index].value;
}

template<typename T>
inline const T& back(const LinkedList<T> &list)
{
    return list.nodes[list.tail_index].value;
}

/**
 * @brief Retruns a pointer to the value at an index, or nullptr if the index is 
 * invalid.
 *
 * Performs bounds checking on the index.
 *
 * Must perform a linear search through the list, either from the start or end 
 * to find the Node.
 * 
 * @param list Pointer to the List.
 * @param index Index into the List.
 *
 * @return T* Pointer to the item at the index, or nullptr if index is invalid. 
 */
template<typename T>
inline T& at(LinkedList<T> &list, Tundra::uint64 index)
{
    if(index < list.num_nodes) 
        { *Internal::get_node(list, index); }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but List size is: \"%llu\".", index, 
        list.num_nodes);
}

template<typename T>
inline const T& at(const LinkedList<T> &list, Tundra::uint64 index)
{
    if(index < list.num_nodes) 
        { *Internal::get_node(list, index); }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but List size is: \"%llu\".", index, 
        list.num_nodes);
}

/**
 * @brief Returns the number of elements in the List.
 * 
 * @param list Read-only pointer to the List.
 * @return Tundra::uint64 Number of elements in the List.
 */
template<typename T>
inline Tundra::uint64 size(const LinkedList<T> &list)
{
    return list.num_nodes;
}

/**
 * @brief Returns the current capacity of the List.
 * 
 * @param list Read-only pointer to the List .
 *
 * @return Tundra::uint64 Number of elements in the List. 
 */
template<typename T>
inline Tundra::uint64 capacity(LinkedList<T> &list)
{
    return list.capacity;
}

// Iterator Methods ------------------------------------------------------------

template<typename T>
inline Iterator<T> begin(const LinkedList<T> &list)
{
    return Iterator<T> {list, list.head_index};
}

template<typename T>
inline Iterator<T> end(const LinkedList<T> &list)
{
    return Iterator<T> {list, Internal::SENTINEL_IDX};
}

template<typename T>
inline bool operator==(const Iterator<T> &first, const Iterator<T> &second)
{
    return first.node_idx == second.node_idx;
}

template<typename T>
inline Iterator<T>& operator++(Iterator<T> &it)
{
    it.node_idx = it.list_ref.nodes[it.node_idx].next;
    return it;
}

template<typename T>
inline Iterator<T> operator++(Iterator<T> &it, int /** postfix */)
{
    Iterator<T> copy;
    ++it;
    return copy;
}

template<typename T>
inline Iterator<T>& operator--(Iterator<T> &it)
{
    it.node_idx = it.list_ref.nodes[it.node_idx].previous;
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
inline T& operator*(const Iterator<T> &it)
{
    return it.list_ref.nodes[it.node_idx].value;
}

} // namespace Tundra::LnkLst
