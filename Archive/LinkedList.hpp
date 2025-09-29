/**
 * @file LinkedList.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing
 *    elements.
 * @version 0.1
 * @date 07-31-25
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

// Default memory alignment.
static constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

// Default capacity in elements of an Array.
static constexpr Tundra::uint8 DEFAULT_CAPACITY = 4;

} // namespace Internal


// Containers ------------------------------------------------------------------

template<typename T>
struct Node
{
    T value;

    // Index inside the List's array of the next Node in the List.
    Tundra::uint64 next;
};

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
    Tundra::LnkLst::Node<T> *data;

    // Index of the tail Node.
    Tundra::uint64 tail_index;

    // Index of the Node before the tail Node.
    Tundra::uint64 index_before_tail;

    // Number of Nodes in the List.
    Tundra::uint64 num_nodes;

    // Current maximum number of elements that the List can store before it 
    // needs to be resized.
    Tundra::uint64 capacity;

    // Indexs inside the Array that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::uint64, alignment> freed_indexes;
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

    list->data = (Tundra::LnkLst::Node<T>*)
        Tundra::alloc_aligned<alignment>(init_capacity * 
        sizeof(Tundra::LnkLst::Node<T>));
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

    Tundra::uint64 new_capacity = 2 * list->capacity;

    // Get a new memory block that is twice the capacity of the current one and 
    // copy over the old bytes.
    Tundra::LnkLst::Node<T> *new_memory = 
        (Tundra::LnkLst::Node<T>*) // Cast
        Tundra::alloc_and_copy_aligned_mem<alignment>(
        (void*)list->data,
        list->num_nodes * sizeof(Tundra::LnkLst::Node<T>),
        new_capacity * sizeof(Tundra::LnkLst::Node<T>));

    Tundra::free_aligned((void*)list->data);
    list->data = new_memory;
    list->capacity = new_capacity;
}

/**
 * @brief Returns the first available index in the array of Nodes of the List.
 * 
 * @param list Read-only pointer to the List.
 *
 * @return Tundra::uint64 First available index.
 */
template<typename T, Tundra::uint8 alignment>
Tundra::uint64 get_available_index(
    const Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    // If there are no available indexes that have been freed.
    if(Tundra::DynStk::is_empty(&list->freed_indexes))
    {
        // Since there are no freed positions inside the array of nodes, the 
        // first available position will always be at the very end of the fully
        // filled array of nodes. 
        return list->num_nodes;
    }

    // -- Fetch an available index from the stack of freed indexes. --
    
    // Use temporary variable since it needs to be stored and then popped.
    Tundra::uint64 available_index = 
        *Tundra::DynStk::front(&list->freed_indexes);
    Tundra::DynStk::pop(&list->freed_indexes);
    
    return available_index;
}

/**
 * @brief Returns a pointer to the Node at an index.
 * 
 * Private method, so we assume a valid non empty List and valid index.
 * 
 * @param list Pointer to the List.
 * @param index Index of the Node.
 *
 * @return Tundra::LnkLst::Node<T>* Pointer to the Node at the index. 
 */
template<typename T, Tundra::uint8 alignment>
Tundra::LnkLst::Node<T>* get_node(
    Tundra::LnkLst::LinkedList<T, alignment> *list, Tundra::uint64 index)
{
    
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a List with default capacity. Allocates memory and 
 * resets internal components.
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
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) * 
        Tundra::LnkLst::Internal::DEFAULT_CAPACITY;

    Tundra::LnkLst::Internal::underlying_init(list, init_capacity);
}

/**
 * @brief Initializes a List with an initial array of elements. Allocates 
 * at least enough memory for `num_elements` and resets internal components.
 *
 * Elements are copied in from `init_elements`.
 *
 * `num_elements` must be greater than 0.
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

    Tundra::uint64 num_copy_bytes = num_elements * sizeof(T);

    // Temporary var for passing a reference to the reserve call to get the new
    // capacity in bytes.
    Tundra::uint64 new_capacity_bytes;

    Tundra::alloc_and_reserve_aligned_mem<alignment>((void**)&list->data, 
        &new_capacity_bytes, num_copy_bytes);

    // Tundra::copy_mem((void*)init_elements, (void*)list->data,
    //     num_copy_bytes);

    Tundra::LnkLst::Node<T> *list_data = list->data;

    // Place first Node since it doesn't need to reset the `next` member of the
    // Node before it.
    list_data[0].value = init_elements[0];

    // Start at 1 since we've already added the initial element.
    for(Tundra::uint64 i = 1; i < num_elements; ++i)
    {
        list_data[i - 1].next = i; // Set previous node to point to this index.
        list_data[i].value = init_elements[i];
    }

    list->num_nodes = num_elements;
    list->tail_index = 0;
    list->capacity = new_capacity_bytes / sizeof(T);
}

/**
 * @brief Releases heap memory allocated for the List.
 *
 * After calling this method, the List should not be used unless reinitialized.
 *
 * It is safe to call this method on a List that has already been freed, or 
 * never initialized. 
 * 
 * @param list Pointer to the List
 */
template<typename T, Tundra::uint8 alignment>
inline void free(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    Tundra::DynStk::free(&list->freed_indexes);

    if(!list->data) { return; }

    Tundra::free_aligned((void*)list->data);
    list->data = NULL;
}

/**
 * @brief Resets the List to an empty state.
 *
 * This does not modify, shrink, deallocate, or zero out the underlying memory.
 * Only the internal tracking elements for the nodes are reset, so subsequent 
 * adds will overwrite previous data.
 * 
 * @param list Pointer the List. 
 */
template<typename T, Tundra::uint8 alignment>
inline void clear(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    Tundra::DynStk::clear(&list->freed_indexes);

    list->num_nodes = 0;
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
        list->data[0].value = *element;
        list->data[0].next = 0;
        ++list->num_nodes;
        return;
    }

    Tundra::LnkLst::Internal::check_and_handle_resize(list);

    Tundra::uint64 available_index = 
        Tundra::LnkLst::Internal::get_available_index(list);

    list->data[available_index].value = *element;
    list->data[available_index].next = 0;

    // Update the tail node to point to this node as next.
    list->data[list->tail_index].next = available_index; 

    // Update tail index to this new position. 
    list->tail_index = available_index;

    ++list->num_nodes;
}

/**
 * @brief Adds an element to the end of List. 
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

/**
 * @brief Removes the element at the specified index, returns true if 
 * successful.
 * 
 * @param list Pointer to the list.
 * @param index Index of the element to remove.
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
        // Replace the head Node with the Node that the head Node was pointing 
        // to. If the head Node was not pointing to anything else, it's "next 
        // Node" would be 0, therefore itself and a simple redundant copy is 
        // made, eliminating a branch statement to check if the head Node was 
        // pointing to anything.
        list->data[0].value = list->data[list->data[0].next].value;

        // Index of the Node the head Node is pointing to.
        Tundra::uint64 next_node_index = list->data[0].next;

        list->data[0].next = list->data[next_node_index].next;

        --list->num_nodes;
        
        // Since we've moved a Node to the head Node position, we need to flag
        // that that position that was moved is now freed and can be reused.
        Tundra::DynStk::push(&list->freed_indexes, next_node_index);
        return true;
    }

    // Previous Node points to the head Node initially.
    Tundra::LnkLst::Node<T> *previous_node = &list->data[0];

    // Index into the array of Nodes of the current parsed Node.
    Tundra::uint64 current_index = previous_node->next;

    // The head Node (`previous_node`) is guaranteed to be pointing to 
    // something, since we have checked that we are not removing the head Node,
    // and the index is greater than num nodes so there is at least another Node
    // in the list.
    Tundra::LnkLst::Node<T> *parsed_node = &list->data[current_index];

    // Iterate until we've reached the target index.
    for(Tundra::uint64 i = 1; i < index; ++i)
    {
        previous_node = parsed_node;
        current_index = previous_node->next;
        parsed_node = &list->data[current_index];
    }

    // -- `parsed_node` now points to the Node we want to remove. --

    // Since we're removing the parsed Node, update the previous Node to point 
    // to what the to-be removed Node is pointing to.
    previous_node->next = parsed_node->next;

    // Flag that the index that we have removed is now available to be reused.
    Tundra::DynStk::push(&list->freed_indexes, current_index);

    --list->num_nodes;
    return true;
}

/**
 * @brief Returns a pointer to the last element in the List.
 *
 * @attention For fast access, this method does not perform a check if the List 
 * is empty. It is the user's responsibilty to ensure the List is not empty.
 * 
 * @param list Pointer to the List.
 *
 * @return T* Pointer to the last element in the List
 */
template<typename T, Tundra::uint8 alignment>
inline T* back(Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return &list->data[list->tail_index];
}

/**
 * @brief Returns a pointer to the value at an index, or NULL if the index is 
 * invalid.
 *
 * Must perform a linear search from the beginning of the List to find the 
 * index.
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

    // Get first Node.
    Tundra::LnkLst::Node<T> *parsed_node = list->data[0];

    for(Tundra::uint64 i = 1; i < index; ++i)
    {
        parsed_node = list->data[parsed_node->next];
    }

    return &parsed_node->value;
}

/**
 * @brief Returns the memory alignment of the List.
 * 
 * @param list Read-only pointer to the List.
 *
 * @return constexpr Tundra::uint8 Memory alignment of the List.
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
 *
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
 * @param list Read-only pointer to List.
 *
 * @return Tundra::uint64 Current capacity of the List.
 */
template<typename T, Tundra::uint8 alignment>
inline Tundra::uint64 capacity(
    const Tundra::LnkLst::LinkedList<T, alignment> *list)
{
    return list->capacity;
}

} // namespace Tundra::LnkLst
