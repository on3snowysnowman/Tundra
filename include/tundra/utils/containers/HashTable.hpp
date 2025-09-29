/**
 * @file HashTable.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Key-value container providing constant-time access through 
 *    customizable hashing.
 * @version 0.1
 * @date 08-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/containers/DynamicStack.hpp"
#include "tundra/utils/memory/MemoryAlloc.hpp"
#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/Hash.hpp"
#include "tundra/utils/memory/MemoryUtils.hpp"


namespace Tundra::HshTbl
{

namespace Internal
{

// Default memory alignment.    
constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

// Default capacity in elements of the HashTable
constexpr Tundra::uint8 DEFAULT_CAPACITY = 16;

// Ratio (out of 10) between used/capacity in the top of a HashTable, where if 
// exceeded will be expanded and rehashed.
constexpr Tundra::uint8 TOP_LIMIT = 7;

// Proportion (out of 10) of the total table capacity that will be allocated for 
// the top of the table, where the remaining space is reserved for the cellar.
constexpr Tundra::uint8 TOP_PROPORTION = 8;


template<typename key_type, typename value_type>
struct Entry
{
    // The key of this Entry. 
    key_type key;

    // The value of this Entry.
    value_type value;

    // Tracks 1 of 3 states the Entry can hold. If this int is -2, the Entry is
    // not initialized/used and a key/value pair can be placed here. If -1, the
    // This Entry is valid and holds an added key/value pair, and no other 
    // entries have been added with a different key but the same hash. If 
    // greater than -1, this int stores the index inside the cellar of the 
    // Entry that collided with this one and had to be stored in the cellar, 
    // creating a "pointer" to it, establishing a chain.
    Tundra::int64 status;

    // Computed hash of this Entry's key.
    Tundra::uint64 hash;
};


} // namespace Internal


// Containers ------------------------------------------------------------------

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*) = Tundra::hash<key_type>, 
    bool (*cmp_func)(const key_type*, const key_type*) = 
        Tundra::is_matching_value<key_type>,
    Tundra::uint8 alignment = Tundra::HshTbl::Internal::DEFAULT_ALIGNMENT
>
struct HashTable 
{
    // Full array of all Entrys.
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *data;

    // Number of entries added to the "top" of the table.
    Tundra::uint64 num_entries_top;

    // Capacity in Entrys of the "top" of the table.
    Tundra::uint64 top_capacity;

    // Capacity in Entrys of the "cellar" of the table.
    Tundra::uint64 cellar_capacity;

    // Index inside the cellar of the next empty position, when the 
    // `available_cellar_indexes` stack is empty.
    Tundra::uint64 next_available_cellar_index;

    // Indexes inside the cellar that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::int64, alignment> 
        available_cellar_indexes;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Forward declaration.
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void transfer_entry_chain(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *old_table, Tundra::HshTbl::HashTable<key_type, 
    value_type, hash_func, cmp_func, alignment> *new_table, int64_t init_index);

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void resize(Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func, alignment> *table)
{
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, cmp_func, 
        alignment> new_table;

    // Double the old capacity.
    Tundra::uint64 new_total_capacity = 
        (table->top_capacity + table->cellar_capacity) * 2;

    new_table.num_entries_top = 0;
    new_table.top_capacity = (new_total_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10;
    new_table.cellar_capacity = new_total_capacity - new_table.top_capacity;
    new_table.next_available_cellar_index = new_table.top_capacity;

    new_table.data = (Tundra::HshTbl::Internal::Entry<key_type, value_type>*)
        Tundra::alloc_aligned<alignment>(new_total_capacity *
        sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
        
    // Set all sets of 8 bytes in the array of Entrys to -2, since we want the 
    // `status` variable of each Entry to be -2, and the other members of each
    // Entry will be overriden later when a valid Entry is placed there.
    Tundra::set_mem_8_bytes((void*)table->data, 
        (sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>) * 
            new_total_capacity) / 8, -2);

    Tundra::DynStk::clear(&table->available_cellar_indexes);
    new_table.available_cellar_indexes = table->available_cellar_indexes;

    // Iterate through each spot in top of the Table.
    for(Tundra::uint64 i = 0; i < table->top_capacity; ++i)
    {
        // If this spot is empty, pass by it.
        if(table->data[i].status <= -2) { continue; }

        // Transfer this Entry and any others in its chain to the new Table.
        Tundra::HshTbl::Internal::transfer_entry_chain(table, &new_table, 
            (Tundra::int64)i);
    }

    Tundra::free_aligned(table->data);
    *table = new_table;
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void underlying_init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, Tundra::uint64 init_capacity)
{
    table->data = (Tundra::HshTbl::Internal::Entry<key_type, value_type>*)
        Tundra::alloc_aligned<alignment>(init_capacity * 
            sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
    
    table->num_entries_top = 0;
    table->top_capacity = (init_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10; // Divide by 10 since 
        // TOP_PROPORTION is a value out of 10.
    table->cellar_capacity = init_capacity - table->top_capacity;
    table->next_available_cellar_index = table->top_capacity;

    // Set all sets of 8 bytes in the top of Entrys to -2, since we want the 
    // `status` variable of each Entry to be -2, and the other members of each
    // Entry will be overriden later when a valid Entry is placed there.
    Tundra::set_mem_8_bytes((void*)table->data, 
        (sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>) * 
            table->top_capacity) / 8, -2);
}

// Forward declaration.
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func, alignment> *table, Tundra::uint64 collided_index, 
    const key_type *key, const value_type *value, Tundra::uint64 hash);

template
<
    typename key_type, typename value_type,
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void underlying_add(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type *key, 
    const value_type *value, Tundra::uint64 hash)
{
    Tundra::uint64 index_into_top = hash % table->top_capacity;

    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &table->data[index_into_top];

    // There is no Entry at the computed index. Place the Entry here.
    if(parsed_entry->status <= -2)
    {
        parsed_entry->key = *key;
        parsed_entry->value = *value;
        parsed_entry->hash = hash;
        parsed_entry->status = -1;

        ++table->num_entries_top;
        return;
    }

    // We have an active Entry at the computed position. Check if its key 
    // matches the same key as the one we are adding.
    if(Tundra::is_matching_value(&parsed_entry->key, key))
    {
        // This Entry shares the key being added, simply update its value.
        parsed_entry->value = *value;
        return;
    }

    // Initial Entry key doesn't match, so we have a collision. Handle it.
    Tundra::HshTbl::Internal::handle_collision(table, index_into_top, key, 
        value, hash);
}


template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func, alignment> *table, Tundra::uint64 collided_index, 
    const key_type *key, const value_type *value, Tundra::uint64 hash)
{
    // If the capacity of the top has breached the limit of the top capacity, 
    // or if the next available cellar index is outside the bounds of the table.
    if(((table->num_entries_top * 10) / table->top_capacity) >=
        Tundra::HshTbl::Internal::TOP_LIMIT || 
        table->next_available_cellar_index >= table->cellar_capacity +
        table->top_capacity)
    {

        Tundra::HshTbl::Internal::resize(table);
        Tundra::HshTbl::Internal::underlying_add(table, key, value, hash);
        return;
    }
    
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &table->data[collided_index];

    // Find the last Entry in the chain by looping until a -1 is found for an 
    // Entry's status, which flags there are no further nodes in the chain.
    while(parsed_entry->status > -1)
        { parsed_entry = &table->data[parsed_entry->status]; }
    
    // Collided index now points to the last Entry in the chain.

    Tundra::uint64 available_index;

    // If there are any recycled slots in the cellar.
    if(!Tundra::DynStk::is_empty(&table->available_cellar_indexes))
    {
        available_index = *Tundra::DynStk::front(
            &table->available_cellar_indexes);
        Tundra::DynStk::pop(&table->available_cellar_indexes);
    }

    // No recycled slots in the cellar, place at the next available cellar 
    // index.
    else
    {
        available_index = table->next_available_cellar_index++;
    }

    // Update the last Entry in the chain to reflect that this position is a 
    // new addition to the Entry chain that share the same hash.
    parsed_entry->status = available_index;

    // Update the Entry pointer to point to the new Entry position.
    parsed_entry = &table->data[available_index];

    parsed_entry->status = -1;
    parsed_entry->hash = hash;
    parsed_entry->key = *key;
    parsed_entry->value = *value;
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void transfer_entry_chain(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *old_table, 
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, cmp_func, 
    alignment> *new_table, int64_t init_index) 
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &old_table->data[init_index];

    // Add the first Entry in the chain.
    Tundra::HshTbl::Internal::underlying_add(new_table, 
        &parsed_entry->key, 
        &parsed_entry->value,
        parsed_entry->hash);

    // Iterate through each Entry in the chain and add it to the new Table.
    while(parsed_entry->status > -1)
    {
        // Update the index to point to the next Entry in the chain.
        init_index = parsed_entry->status;

        Tundra::HshTbl::Internal::underlying_add(new_table, 
            &parsed_entry->key,
            &parsed_entry->value,
            parsed_entry->hash);
    }
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline value_type* get_value_of_key(const Tundra::HshTbl::HashTable<key_type, 
    value_type, hash_func, cmp_func, alignment> *table, const key_type *key)
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry =
        &table->data[hash_func(key) % table->top_capacity];

    // The Entry at the computed hash index is empty 
    if(parsed_entry->status <= -2) { return NULL; }

    // -- The Entry is not empty, check for key matches. --

    // If the initial Entry is the key we're searching for.
    if(cmp_func(&parsed_entry->key, key))
        { return &parsed_entry->value; }
    
    // -- The Entry is somewhere after the initial Entry in the chain.. --

    // Iterate over the chain and check for a match to the key we're looking 
    // for.
    while(parsed_entry->status > -1)
    {
        parsed_entry = &table->data[parsed_entry->status];

        // If we have a key match.
        if(cmp_func(&parsed_entry->key, key))
            { return &parsed_entry->value; }
    }
    
    // If we've reached this point, we're at the end of the chain and the key 
    // hasn't been found.
    return NULL;
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a HashTable with default capacity. Allocates memory and 
 * resets internal components.
 *
 * @param table Pointer to the Table.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
void init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table)
{
    Tundra::HshTbl::Internal::underlying_init(table, 
        (Tundra::uint64)Tundra::HshTbl::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes a HashTable with a specified capacity. Allocates memory 
 * and resets internal components.
 *
 * If `init_capacity` is 0, it is set to the default capacity.
 *
 * @param table Pointer to the Table.
 * @param init_capacity Initial capacity in elements.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
void init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) *
        Tundra::HshTbl::Internal::DEFAULT_CAPACITY;

    Tundra::HshTbl::Internal::underlying_init(table, 
        init_capacity);
}

/**
 * @brief Releases heap memory allocated for the HashTable.
 * 
 * After calling this method, the Table should not be used unless reinitialized.
 *
 * It is safe to call this method on a Table that has already been freed, or 
 * never initialized.
 *
 * @param table Pointer to the Table.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
void free(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table)
{
    Tundra::DynStk::free(&table->available_cellar_indexes);

    if(!table->data) { return; }

    Tundra::free_aligned(table->data);
    table->data = NULL;
}

/**
 * @brief Clears the Table of Entrys.
 *
 * @param table Pointer to the Table.
  */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void clear(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table)
{
    // Set all sets of 8 bytes in the array of Entrys to -2, since we want the 
    // `status` variable of each Entry to be -2, and the other members of each
    // Entry will be overriden later when a valid Entry is placed there.
    Tundra::set_mem_8_bytes((void*)table->data, 
        (sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>) * 
            table->top_capacity) / 8, -2);

    table->num_entries_top = 0;

    // Reset the cellar index to 0 by setting it to the end of the top, which
    // is where the cellar starts.
    table->next_available_cellar_index = table->top_capacity;
    
    Tundra::DynStk::clear(&table->available_cellar_indexes);
}

/**
 * @brief Adds a a pair to the Table, where `value` is associated to the hash 
 * of `key`.
 *
 * If the key/value pair already exists in the table, the value is replaced with
 * `value`.
 *
 * @param key Read-only pointer to the key of the pair, used as the lookup for 
 *    `value`.
 * @param value Read-only pointer to the value of the pair, retrieved when `key` 
 *    is looked up.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void add(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type *key,
    const value_type *value) 
{
    Tundra::HshTbl::Internal::underlying_add(table, key, value, hash_func(key));
}

/**
 * @brief Adds a a pair to the Table, where `value` is associated to the hash 
 * of `key`.
 *
 * If the key/value pair already exists in the table, the value is replaced with
 * `value`.
 *
 * @param key The key of the pair, used as the lookup for `value.
 * @param value The value of the pair, retrieved when `key` is looked up.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline void add(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type key,
    const value_type value) 
{
    Tundra::HshTbl::Internal::underlying_add(table, &key, &value, 
        hash_func(&key));
}

/**
 * @brief Removes the key/value pair associated with the given key from the 
 * Table. Returns true if the erasure was successful.
 *
 * @param table Pointer to the Table.
 * @param key Read-only pointer to the key use to look up the pair to erase.
 *
 * @return bool True if successful.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline bool erase(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type *key)
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *table_data = 
        table->data;

    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &table_data[hash_func(key) % table->top_capacity];

    Tundra::int64 parsed_entry_status = parsed_entry->status;

    // If there is no Entry at the hashed index.
    if(parsed_entry_status <= -2) { return false; }

    // -- Check the first Entry, since it is a special case in which it is known
    // that there are no previous entries that point to it that need updated if 
    // it is removed. --

    // If the first Entry is the key we're looking for.
    if(cmp_func(&parsed_entry->key, key))
    {
        // If this Entry is not pointing to any Entry in the cellar, we can just
        // remove it.
        if(parsed_entry_status <= -1)
        {
            parsed_entry->status = -2;
            return true;
        }

        // -- The first Entry is pointing to an Entry in the cellar, so we need 
        // to move that Entry to this position inside the top since we're 
        // removing the Entry from the top. --

        // Add the index into the cellar of the Entry that is to be moved to 
        // the stack of available cellar indexes, since the spot is going to be
        // freed.
        Tundra::DynStk::push(&table->available_cellar_indexes,
            parsed_entry_status);

        // Move the next entry from the cellar to this position in the top.
        *parsed_entry = table_data[parsed_entry_status];
        parsed_entry_status = parsed_entry->status;

        return true;
    }

    // While there is another Entry in the chain.
    while(parsed_entry->status > -1)
    {
        // Save the previous Entry so that if we remove an Entry, we can update
        // the previous one to point to the Entry that the removed Entry points 
        // to, like removing a node in a linked list.
        Tundra::HshTbl::Internal::Entry<key_type, value_type>
            *previous_entry = parsed_entry;
        
        // Grab the next Entry in the chain.
        parsed_entry = &table_data[parsed_entry->status];

        // If this is not the key we're looking for.
        if(!cmp_func(&parsed_entry->key, key)) { continue; }

        // -- This is the key we're looking for. -- 

        // Add the index of the cellar entry that is being removed to the stack
        // of available cellar indexes. The previous Entry's status is used here
        // since it contains the index of the currently parsed Entry.
        Tundra::DynStk::push(&table->available_cellar_indexes, 
            (Tundra::int64)previous_entry->status);
            
        // Update the previous Entry in the chain to point to what this Entry
        // is pointing to, since we're removing this Entry.
        // table->data[previous_entry_index].status =
        //     table->data[entry_index].status;
        previous_entry->status = parsed_entry->status;

        // Set the status of the Entry to -2 to mark it as empty, effectively
        // "removing" it from the Table. #TODO: Maybe we don't need this?
        // table->data[entry_index].status = -2;
        
        return true;
    }

    // We've iterated over the entire chain, the key was not found.
    return false;
}

/**
 * @brief Removes the key/value pair associated with the given key from the 
 * Table. Returns true if the erasure was successful.
 *
 * @param table Pointer to the Table.
 * @param key The key use to look up the pair to erase.
 *
 * @return bool True if successful.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline bool erase(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type key)
{
    return Tundra::HshTbl::erase(table, &key);
}

/*
 * @brief Checks if there is an Entry in the Table with the passed `key`, 
 * returning true if there is.
 *
 * @param table Read-only pointer to the table.
 * @param key Read-only pointer to the key to look for.
 *
 * @return bool True if there is an Entry in the Table with the key.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline bool contains(const Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type *key)
{
    return Tundra::HshTbl::Internal::get_value_of_key(table, key) != NULL;
}

/*
 * @brief Checks if there is an Entry in the Table with the passed `key`, 
 * returning true if there is.
 *
 * @param table Read-only pointer to the table.
 * @param key Key to look for.
 *
 * @return bool True if there is an Entry in the Table with the key.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline bool contains(const Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type key)
{
    return Tundra::HshTbl::Internal::get_value_of_key(table, &key) != NULL;
}

/**
 * @brief Returns a pointer to the value tied to `key` in the table, NULL if the
 * key/value pair does not exist.
 *
 * @param table Read-only pointer to the Table
 * @param key Read-only pointer to the key to find the value of.
 *
 * @return value_type* Pointer to the value associated with `key`, or NULL if 
 * the key/value pair doesn't exist.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline value_type* at(const Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type *key)
{
    return Tundra::HshTbl::Internal::get_value_of_key(table, key);
}

/**
 * @brief Returns a pointer to the value tied to `key` in the table, NULL if the
 * key/value pair does not exist.
 *
 * @param table Read-only pointer to the Table
 * @param key The key to find the value of.
 *
 * @return value_type* Pointer to the value associated with `key`, or NULL if 
 * the key/value pair doesn't exist.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*),
    bool (*cmp_func)(const key_type*, const key_type*), 
    Tundra::uint8 alignment
>
inline value_type* at(const Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func, alignment> *table, const key_type key)
{
    return Tundra::HshTbl::Internal::get_value_of_key(table, &key);
}

} // namespace Tundra::HshTbl
