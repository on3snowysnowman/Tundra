/**
 * @file LinkedList.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing 
 *    elements.
 * @version 0.1
 * @date 08-13-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/containers/DynamicStack.hpp"


namespace Tundra::LnkLst
{

namespace Internal
{

constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;
constexpr Tundra::uint8 DEFAULT_CAPACITY = 4;

// Flag for a Node's `next` or `previous` member for when the Node does not have 
// a next or previous Node.
constexpr Tundra::int64 NO_NODE = -1;

}; // namespace Internal


// Containers ------------------------------------------------------------------

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
    // next Node, this value will be `Tundra::LnkLst::Internal::NO_NODE`.
    Tundra::int64 next;

    // Index into the List's array of the previous Node in the List. If there is
    // no previous Node, this value will be `Tundra::LnkLst::Internal::NO_NODE`.
    Tundra::int64 previous;
};

/**
 * @brief Dynamically linked container for efficiently inserting and removing 
 *    elements.
 * 
 * @tparam T Type the List stores.
 * @tparam alignment Alignment in bytes to align the List's heap memory. 
 *    (allows SIMD instruction use for fast reallocation).
 */
template<typename T, 
    Tundra::uint8 alignment = Tundra::LnkLst::Internal::DEFAULT_ALIGNMENT> 
struct LinkedList 
{
    // Array of Nodes. 
    Tundra::LnkLst::Node<T> *nodes;

    // Index of the head Node.
    Tundra::uint64 head_index;

    // // Index of the Node pointing to the tail Node.
    // Tundra::uint64 second_tail_index;

    // Index of the tail Node.
    Tundra::uint64 tail_index;

    // Number of Nodes in the List.
    Tundra::uint64 num_nodes;

    // Current maximum number of elements that the List can store before it 
    // needs to be resized.
    Tundra::uint64 capacity;

    // Indexes inside the Array that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::int64> freed_indexes;
};


// Internal Methods ---------------------------------------------------------------

namespace Internal
{

/**
 * @brief Underlying initialization method. Allocates initial memory for 
 * `init_capacity` elements and sets internal values.
 * 
 * @param list Pointer to the List.
 * @param init_capacity Initial capacity in elements to allocate.
 */
template<typename T, Tundra::uint8 alignment>
inline void underlying_init(Tundra::LnkLst::LinkedList<T, alignment> *list, 
    Tundra::uint64 init_capacity)
{
    Tundra::DynStk::init(&list->freed_indexes);

    list->nodes = (Tundra::LnkLst::Node<T>*)Tundra::alloc_aligned<alignment>(
        init_capacity * sizeof(Tundra::LnkLst::Node<T>));

    list->head_index = 0;
    list->tail_index = 0;
    list->num_nodes = 0;
    list->capacity = init_capacity;
}   

/**
 * @brief Checks if the List has filled its allocated capacity, resizing if it
 * has.
 *  
 * @param list Pointer to the List.
 */
template<typename T, Tundra::uint8 alignment>
inline void check_and_handle_resize(
    Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    if(list->num_nodes < list->capacity) { return; }

    const Tundra::uint64 NEW_CAPACITY = 2 * list->capacity;

    // Get a new memory block that is twice the capacity of the current one and
    // copy over the old bytes.
    Tundra::LnkLst::Node<T> *new_memory =
        (Tundra::LnkLst::Node<T>*) // Cast
        Tundra::alloc_and_copy_aligned_mem<alignment>(
            (void*)list->nodes,
            list->num_nodes * sizeof(Tundra::LnkLst::Node<T>),
            NEW_CAPACITY * sizeof(Tundra::LnkLst::Node<T>));
    
    Tundra::free_aligned((void*)list->nodes);
    list->nodes = new_memory;
    list->capacity = NEW_CAPACITY;
}

template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 get_available_index(
    Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    // If there are no available indexes that have been freed.
    if(Tundra::DynStk::is_empty(&list->freed_indexes))
    {
        // Since there are no freed positions inside the array of Nodes, the 
        // first available position will always be at the end of the filled 
        // array of Nodes.
        return list->num_nodes;
    }

    // -- Fetch an available index from the stack of freed indexes. --

    Tundra::uint64 available_index =
        *Tundra::DynStk::peek_unchecked(&list->freed_indexes);
    Tundra::DynStk::pop(&list->freed_indexes);
    return available_index;
}

template<typename T, Tundra::uint8 alignment>
inline Tundra::LnkLst::Node<T>* find_from_start(
    Tundra::LnkLst::LinkedList<T, alignment> *list, Tundra::uint64 index) 
{
    Tundra::LnkLst::Node<T> *parsed_node = &list->nodes[list->head_index];

    // Iterate through each Node in the List until we've reached the index.
    // Start at iteration 1 since we've already parsed the head Node.
    for(Tundra::uint64 i = 1; i <= index; ++i)
    {
        parsed_node = &list->nodes[parsed_node->next];
    }

    return parsed_node;
}

template<typename T, Tundra::uint8 alignment>
inline Tundra::LnkLst::Node<T>* find_from_end(
    Tundra::LnkLst::LinkedList<T, alignment> *list, Tundra::uint64 index) 
{
    Tundra::LnkLst::Node<T> *parsed_node = &list->nodes[list->tail_index];

    // Since `index` gives us a parse amount from the start of the list, we 
    // need to calculate the index to stop at with respect to starting at the 
    // end of the List.
    const Tundra::uint64 INDEX_FROM_END = list->num_nodes - index;

    // Iterate backwards through each Node in the List until we've reached the
    // index. Start at iteration 1 since we've already parsed the tail Node.
    for(Tundra::uint64 i = 1; i < INDEX_FROM_END; i--)
    {
        parsed_node = &list->nodes[parsed_node->previous];
    }

    return parsed_node;
}

/**
 * @brief Returns a pointer to the Node at the index.
 * 
 * @param list Pointer to the List. 
 * @param index Index of the Node.
 *
 * @return Tundra::LnkLst::Node<T>* Pointer to the Node at `index`.
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::LnkLst::Node<T>* get_node(
    Tundra::LnkLst::LinkedList<T, alignment> *list, Tundra::uint64 index)
{
    // If the index is in the first half of the list, iterate from the start. 
    // Otherwise, iterate from the end.
    return (index < list->num_nodes / 2) ? 
        Tundra::LnkLst::Internal::find_from_start(list, index) : 
        Tundra::LnkLst::Internal::find_from_end(
            list, list->num_nodes - index - 1);
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a List with default capacity. Allocates memory and resets
 * internal components.
 * 
 * @param list Pointer to the List.
 */
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    Tundra::LnkLst::Internal::underlying_init(list, 
        Tundra::LnkLst::Internal::DEFAULT_CAPACITY);
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
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::LnkLst::LinkedList<T, alignment> *list,
    Tundra::uint64 init_capacity)
{
    init_capacity = (init_capacity == 0) ? 
        Tundra::LnkLst::Internal::DEFAULT_CAPACITY : init_capacity;

    Tundra::LnkLst::Internal::underlying_init(list, init_capacity);
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
template<typename T, Tundra::uint8 alignment>
inline void init(Tundra::LnkLst::LinkedList<T, alignment> *list,
    const T *init_elements, Tundra::uint64 num_elements)
{
    Tundra::DynStk::init(&list->freed_indexes);

    // New capacity in bytes of the List, will be set by the following 
    // reserve call.
    Tundra::uint64 new_capacity_bytes;

    const Tundra::uint64 NUM_RESERVE_BYTES = 
        num_elements * sizeof(Tundra::LnkLst::Node<T>);

    Tundra::alloc_and_reserve_aligned_mem<alignment>((void**)list->nodes,
        &new_capacity_bytes, NUM_RESERVE_BYTES);
    list->capacity = new_capacity_bytes / sizeof(T);
    
    // Place the first Node since it doesn't need to reset the `next` member of 
    // the Node before it.
    list->nodes[0].value = init_elements[0];
    list->nodes[0].previous = Tundra::LnkLst::Internal::NO_NODE;

    // Start at 1 since we've added the initial Node.
    Tundra::uint64 i = 1;
    while(i < num_elements)
    {
        list->nodes[i - 1].next = i;
        list->nodes[i].previous = i - 1;
        list->nodes[i].value = init_elements[i];
        ++i;
    }

    // Set the last added Node's next value since it does not have a next Node.
    list->nodes[i - 1].next = Tundra::LnkLst::Internal::NO_NODE;

    list->head_index = 0;
    list->tail_index = 0;
    list->num_nodes = num_elements;
}

/**
 * @brief Released heap memory allocated for the List.
 * 
 * @param list Pointer to the List. 
 */
template<typename T, Tundra::uint8 alignment>
inline void free(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    Tundra::DynStk::free(&list->freed_indexes);

    if(!list->nodes) { return; }

    Tundra::free_aligned((void*)list->nodes);
    list->nodes = NULL;
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
template<typename T, Tundra::uint8 alignment>
inline void clear(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    Tundra::DynStk::clear(&list->freed_indexes);

    list->num_nodes = 0;
    list->head_index = 0;
    list->tail_index = 0;
}

/**
 * @brief Adds an element to the end of the List.
 * 
 * @param list Pointer to the List. 
 * @param element Read-only pointer to the element to add.
 */
template<typename T, Tundra::uint8 alignment>
inline void add_to_end(Tundra::LnkLst::LinkedList<T, alignment> *list,  
    const T *element)
{
    if(list->num_nodes == 0)
    {
        list->head_index = 0;
        list->nodes[0].value = *element;
        list->nodes[0].next = Tundra::LnkLst::Internal::NO_NODE;
        list->nodes[0].previous = Tundra::LnkLst::Internal::NO_NODE;
        ++list->num_nodes;
        return;
    }

    Tundra::LnkLst::Internal::check_and_handle_resize(list);

    Tundra::uint64 available_index = 
        Tundra::LnkLst::Internal::get_available_index(list);

    list->nodes[available_index].value = *element;
    list->nodes[available_index].next = Tundra::LnkLst::Internal::NO_NODE;
    list->nodes[available_index].previous = list->tail_index;

    // Update the tail node to point to this Node as next.
    list->nodes[list->tail_index].next = available_index;

    // Update the second tail as what the tail is now.
    // list->second_tail_index = list->tail_index;

    // Update tail index to the new position.
    list->tail_index = available_index;

    ++list->num_nodes;
}

/**
 * @brief Adds an element to the end of the List.
 * 
 * @param list Pointer to the List. 
 * @param element Element to add.
 */
template<typename T, Tundra::uint8 alignment>
inline void add_to_end(Tundra::LnkLst::LinkedList<T, alignment> *list,  
    const T element)
{
    Tundra::LnkLst::add_to_end(list, &element);
}

template<typename T, Tundra::uint8 alignment>
inline bool insert(Tundra::LnkLst::LinkedList<T, alignment> *list, 
    const T* element, Tundra::uint64 index)
{
    if(index >= list->num_nodes) { return false; }

    // The index is the last position in the list.
    if(index == list->num_nodes - 1)
    {
        // Simply add to the end.
        Tundra::LnkLst::add_to_end(list, element);
        return true;
    }

    Tundra::LnkLst::Internal::check_and_handle_resize(list);

    // The current Node at the index.
    Tundra::LnkLst::Node<T> *targ_node = 
        Tundra::LnkLst::Internal::get_node(list, index);
    
    Tundra::uint64 avail_index = 
        Tundra::LnkLst::Internal::get_available_index(list);

    // The new Node being inserted in the List.
    Tundra::LnkLst::Node<T> *new_node = 
        &list->nodes[avail_index];

    Tundra::LnkLst::Node<T> *ahead_node = 
        &list->nodes[targ_node->next];

    // Update the new Node to point forward to the Node ahead of the current 
    // Node.
    new_node->next = targ_node->next;

    // Update the new Node to point backward to the current Node.
    new_node->previous = ahead_node->previous;

    // Update the current Node to point to the new Node
    targ_node->next = avail_index;

    // Update the Node ahead of the current Node to point backward to the new
    // Node.
    ahead_node->previous = avail_index;

    ++list->num_nodes;

    return true;
}

template<typename T, Tundra::uint8 alignment>
inline bool insert(Tundra::LnkLst::LinkedList<T, alignment> *list, 
    const T element, Tundra::uint64 index)
{
    return Tundra::LnkLst::insert(list, &element, index);
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
template<typename T, Tundra::uint8 alignment>
inline bool erase(Tundra::LnkLst::LinkedList<T, alignment> *list,
    Tundra::uint64 index)
{
    if(index >= list->num_nodes) { return false; }

    // If we are erasing the head Node.
    if(index == 0)
    {
        // Set the head index to the index that the head Node was pointing to.
        list->head_index = list->nodes[list->head_index].next;
        --list->num_nodes;
        return true;
    }

    // Previous Node points to the head Node initially, since we've already 
    // checked that we are not removing it.
    Tundra::LnkLst::Node<T> *previous_node = list->nodes[list->head_index];
    Tundra::LnkLst::Node<T> *parsed_node = list->nodes[previous_node->next];

    // Iterate until we've reached the target index.
    for(Tundra::uint64 i = 1; i < index; ++i)
    {
        previous_node = parsed_node;
        parsed_node = &list->nodes[parsed_node->next];
    }

    // -- `parsed_node` now points to the Node we want to remove. --

    // Add the index of the Node we are removing to the stack of freed indexes.
    Tundra::DynStk::push(&list->freed_indexes, previous_node->next);

    // Update the previous Node to point to what the parsed Node is pointing to,
    // since we're removing the parsed Node.
    previous_node->next = parsed_node->next;

    --list->num_nodes;
    return true;
}

/**
 * @brief Removes the element at the back of the List, returning true if 
 * successful.
 *
 * Performs a check if the List has a back element to remove.
 * 
 * @param list Pointer to the List.
 * 
 * @return bool True if erasure was successful, false otherwise.
 */
template<typename T, Tundra::uint8 alignment> 
inline bool erase_back(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    if(!(bool)list->num_nodes) { return false; }

    // The List only contains the head Node, so no index pointers need to be 
    // updated, only removed.
    if(list->num_nodes == 1)
    {
        --list->num_nodes;
        // Head index and Tail index are already at 0.
        return true;
    }

    // Index of the Node that is second to the tail.
    Tundra::uint64 second_tail_index = 
        list->nodes[list->tail_index].previous;

    // Update the second to last node to point to nothing.
    list->nodes[second_tail_index].next = Tundra::LnkLst::Internal::NO_NODE;

    list->tail_index = second_tail_index;

    --list->num_nodes; 
    return true;
}

/**
 * @brief Returns a pointer to the last element in the List.
 *
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list Pointer to the List.
 * 
 * @return T* Pointer to the last element in the List. 
 */
template<typename T, Tundra::uint8 alignment>
inline T* back(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return &list->nodes[list->tail_index];
}

/**
 * @brief Retruns a pointer to the value at an index, or NULL if the index is 
 * invalid.
 *
 * Must perform a linear search through the list, either from the start or end 
 * to find the Node.
 * 
 * @param list Pointer to the List.
 * @param index Index into the List.
 *
 * @return T* Pointer to the item at the index, or NULL if index is invalid. 
 */
template<typename T, Tundra::uint8 alignment>
inline T* at(Tundra::LnkLst::LinkedList<T, alignment> *list,
    Tundra::uint64 index)
{
    if(index >= list->num_nodes) { return NULL; }

    return Tundra::LnkLst::Internal::get_node(list, index);

    // If the index is in the first half of the list, iterate from the start. 
    // Otherwise, iterate from the end.
    // return (index < list->num_nodes / 2) ? 
    //     Tundra::LnkLst::Internal::find_from_start(list, index)->value : 
    //     Tundra::LnkLst::Internal::find_from_end(
    //         list, list->num_nodes - index - 1)->value;

    // Get the head Node, which is guaranteed to exist due to the check above 
    // this line.
    // Tundra::LnkLst::Node<T> *parsed_node = &list->nodes[list->head_index];

    // // Iterate through each Node in the List until we've reached the index.
    // // Start at index 1 since we've already parsed the head Node.
    // for(Tundra::uint64 i = 1; i < index; ++i)
    // {
    //     parsed_node = list->nodes[parsed_node->next];
    // }

    // return &parsed_node->value;
}

/**
 * @brief Returns the memory alignment of the List.
 * 
 * @param list Read-only pointer to the List.
 * 
 * @return Tundra::uint8 Memory alignment of the List.
 */
template<typename T, Tundra::uint8 alignment>
constexpr Tundra::uint8 get_alignment(
    const Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return alignment;
}

/**
 * @brief Returns the number of elements in the List.
 * 
 * @param list Read-only pointer to the List.
 * @return Tundra::uint64 Number of elements in the List.
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 size(const Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return list->num_nodes;
}

/**
 * @brief Returns the current capacity of the List.
 * 
 * @param list Read-only pointer to the List .
 *
 * @return Tundra::uint64 Number of elements in the List. 
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 capacity(
    Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return list->capacity;
}


} // namespace Tundra::LnkLst
