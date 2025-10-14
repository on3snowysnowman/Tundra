/**
 * @file HashTable.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Key-value container providing constant-time access through 
 *    customizable hashing.
 * @version 0.1
 * @date 08-01-25
 *
 * @copyright Copyright (c) 2025
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

// Default capacity in elements of the HashTable
constexpr Tundra::uint8 DEFAULT_CAPACITY = 16;

// Ratio (out of 10) between used/capacity in the top of a HashTable, where if 
// exceeded will be expanded and rehashed.
constexpr Tundra::uint8 TOP_LIMIT = 7;

// Proportion (out of 10) of the total tbl capacity that will be allocated for 
// the top of the tbl, where the remaining space is reserved for the cellar.
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
    Tundra::uint64 (*hash_func)(const key_type&) = Tundra::hash<key_type>, 
    bool (*cmp_func)(const key_type&, const key_type&) = 
        Tundra::is_matching_value<key_type>
>
struct HashTable 
{
    // Full array of all Entrys.
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *data;

    // Number of entries added to the "top" of the tbl.
    Tundra::uint64 num_entries_top;

    // Capacity in Entrys of the "top" of the tbl.
    Tundra::uint64 top_capacity;

    // Capacity in Entrys of the "cellar" of the tbl.
    Tundra::uint64 cellar_capacity;

    // Index inside the cellar of the next empty position, when the 
    // `available_cellar_indexes` stack is empty.
    Tundra::uint64 next_available_cellar_index;

    // Indexes inside the cellar that have been freed and can be reused.
    Tundra::DynStk::DynamicStack<Tundra::int64> available_cellar_indexes;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Forward declaration.
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline void transfer_entry_chain(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &old_table, Tundra::HshTbl::HashTable<key_type, 
    value_type, hash_func, cmp_func> &new_table, int64_t init_index);

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool resize(Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func> &tbl)
{
    Tundra::HshTbl::HashTable<key_type, 
        value_type, hash_func, cmp_func> new_table;

    // Double the old capacity.
    Tundra::uint64 new_total_capacity = 
        (tbl.top_capacity + tbl.cellar_capacity) * 2;

    new_table.num_entries_top = 0;
    new_table.top_capacity = (new_total_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10;
    new_table.cellar_capacity = new_total_capacity - new_table.top_capacity;
    new_table.next_available_cellar_index = new_table.top_capacity;

    new_table.data = (Tundra::HshTbl::Internal::Entry<key_type, value_type>*)
        malloc(new_total_capacity *
        sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
        
    if(new_table.data == nullptr) { return false; }

    // Iterate through each Entry in the new table and set its status to -2.
    for(Tundra::uint64 i = 0; i < new_table.top_capacity; ++i)
    {
        new_table.data[i].status = -2;
    }

    // Iterate through each Entry in the top of the table and move it's Entry
    // chain.
    for(Tundra::uint64 i = 0; i < tbl.top_capacity; ++i)
    {
        Tundra::HshTbl::Internal::transfer_entry_chain(tbl, new_table, i);
    }

    Tundra::DynStk::clear(tbl.available_cellar_indexes);
    new_table.available_cellar_indexes = tbl.available_cellar_indexes;

    ::free(tbl.data);
    tbl = new_table;
    return true;
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool underlying_init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, Tundra::uint64 init_capacity)
{
    tbl.data = (Tundra::HshTbl::Internal::Entry<key_type, value_type>*)
        malloc(init_capacity * 
            sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
    
    if(tbl.data == nullptr) { return false; }

    tbl.num_entries_top = 0;
    tbl.top_capacity = (init_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10; // Divide by 10 since 
        // TOP_PROPORTION is a value out of 10.
    tbl.cellar_capacity = init_capacity - tbl.top_capacity;
    tbl.next_available_cellar_index = tbl.top_capacity;

    // Set each Entry's status to -2 to flag it's an empty spot.
    for(Tundra::uint64 i = 0; i < tbl.top_capacity; ++i)
    {
        tbl.data[i].status = -2;
    }
        
    return true;
}

// Forward declaration.
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline void handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func> &tbl, Tundra::uint64 collided_index, 
    const key_type &key, const value_type &value, Tundra::uint64 hash);

template
<
    typename key_type, typename value_type,
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool underlying_add(Tundra::HshTbl::HashTable<key_type, value_type,
    hash_func, cmp_func> &tbl, const key_type &key, 
    const value_type &value, Tundra::uint64 hash)
{
    Tundra::uint64 index_into_top = hash % tbl.top_capacity;

    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &tbl.data[index_into_top];

    // There is no Entry at the computed index. Place the Entry here.
    if(parsed_entry->status <= -2)
    {
        parsed_entry->key = key;
        parsed_entry->value = value;
        parsed_entry->hash = hash;
        parsed_entry->status = -1;

        ++tbl.num_entries_top;
        return true;
    }

    // We have an active Entry at the computed position. Check if its key 
    // matches the same key as the one we are adding.
    if(Tundra::is_matching_value(parsed_entry->key, key))
    {
        // This Entry shares the key being added, simply update its value.
        parsed_entry->value = value;
        return true;
    }

    // Initial Entry key doesn't match, so we have a collision. Handle it.
    return Tundra::HshTbl::Internal::handle_collision(tbl, index_into_top, key, 
        value, hash);
}


template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    cmp_func> &tbl, Tundra::uint64 collided_index, 
    const key_type &key, const value_type &value, Tundra::uint64 hash)
{
    // If the capacity of the top has breached the limit of the top capacity, 
    // or if the next available cellar index is outside the bounds of the tbl.
    if(((tbl.num_entries_top * 10) / tbl.top_capacity) >=
        Tundra::HshTbl::Internal::TOP_LIMIT || 
        tbl.next_available_cellar_index >= tbl.cellar_capacity +
        tbl.top_capacity)
    {
        if(!Tundra::HshTbl::Internal::resize(tbl)) { return false; }
        Tundra::HshTbl::Internal::underlying_add(tbl, key, value, hash);
        return true;
    }
    
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &tbl.data[collided_index];

    // Find the last Entry in the chain by looping until a -1 is found for an 
    // Entry's status, which flags there are no further nodes in the chain.
    while(parsed_entry->status > -1)
        { parsed_entry = &tbl.data[parsed_entry->status]; }
    
    // Collided index now points to the last Entry in the chain.

    Tundra::uint64 available_index;

    // If there are any recycled slots in the cellar.
    if(!Tundra::DynStk::is_empty(tbl.available_cellar_indexes))
    {
        available_index = Tundra::DynStk::front(tbl.available_cellar_indexes);
        Tundra::DynStk::pop(tbl.available_cellar_indexes);
    }

    // No recycled slots in the cellar, place at the next available cellar 
    // index.
    else
    {
        available_index = tbl.next_available_cellar_index++;
    }

    // Update the last Entry in the chain to reflect that this position is a 
    // new addition to the Entry chain that share the same hash.
    parsed_entry->status = available_index;

    // Update the Entry pointer to point to the new Entry position.
    parsed_entry = &tbl.data[available_index];

    parsed_entry->status = -1;
    parsed_entry->hash = hash;
    parsed_entry->key = key;
    parsed_entry->value = value;
    return true;
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool transfer_entry_chain(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &old_table, 
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, cmp_func> 
    &new_table, int64_t init_index) 
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &old_table.data[init_index];

    // Add the first Entry in the chain.
    if(!Tundra::HshTbl::Internal::underlying_add(new_table, 
        parsed_entry->key, 
        parsed_entry->value,
        parsed_entry->hash)) {return false; }

    // Iterate through each Entry in the chain and add it to the new Table.
    while(parsed_entry->status > -1)
    {
        // Update the parsed Entry to point to the next Entry in the chain.
        parsed_entry = &old_table.data[parsed_entry->status];

        if(!Tundra::HshTbl::Internal::underlying_add(new_table, 
            parsed_entry->key,
            parsed_entry->value,
            parsed_entry->hash)) { return false; }
    }

    return true;
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline value_type* get_value_of_key(const Tundra::HshTbl::HashTable<key_type, 
    value_type, hash_func, cmp_func> &tbl, const key_type &key)
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry =
        &tbl.data[hash_func(key) % tbl.top_capacity];

    // The Entry at the computed hash index is empty 
    if(parsed_entry->status <= -2) { return nullptr; }

    // -- The Entry is not empty, check for key matches. --

    // If the initial Entry is the key we're searching for.
    if(cmp_func(parsed_entry->key, key))
        { return &parsed_entry->value; }
    
    // -- The Entry is somewhere after the initial Entry in the chain.. --

    // Iterate over the chain and check for a match to the key we're looking 
    // for.
    while(parsed_entry->status > -1)
    {
        parsed_entry = &tbl.data[parsed_entry->status];

        // If we have a key match.
        if(cmp_func(parsed_entry->key, key))
            { return &parsed_entry->value; }
    }
    
    // If we've reached this point, we're at the end of the chain and the key 
    // hasn't been found.
    return nullptr;
}

} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a HashTable with default capacity. Allocates memory and 
 * resets internal components.
 *
 * @param tbl Pointer to the Table.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
bool init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl)
{
    return Tundra::HshTbl::Internal::underlying_init(tbl, 
        (Tundra::uint64)Tundra::HshTbl::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes a HashTable with a specified capacity. Allocates memory 
 * and resets internal components.
 *
 * If `init_capacity` is 0, it is set to the default capacity.
 *
 * @param tbl Pointer to the Table.
 * @param init_capacity Initial capacity in elements.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
bool init(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) *
        Tundra::HshTbl::Internal::DEFAULT_CAPACITY;

    return Tundra::HshTbl::Internal::underlying_init(tbl, init_capacity);
}

/**
 * @brief Releases heap memory allocated for the HashTable.
 * 
 * After calling this method, the Table should not be used unless reinitialized.
 *
 * It is safe to call this method on a Table that has already been freed, or 
 * never initialized.
 *
 * @param tbl Pointer to the Table.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
void free(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl)
{
    Tundra::DynStk::free(tbl.available_cellar_indexes);

    ::free(tbl.data);
    tbl.data = nullptr;
}

/**
 * @brief Clears the Table of Entrys.
 *
 * @param tbl Pointer to the Table.
  */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline void clear(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl)
{
    // Set each Entry's status in the table to -2 to flag it as empty.
    for(Tundra::uint64 i = 0; i < tbl.top_capacity; ++i)
    {
        tbl.data[i].status = -2;
    }

    tbl.num_entries_top = 0;

    // Reset the cellar index to 0 by setting it to the end of the top, which
    // is where the cellar starts.
    tbl.next_available_cellar_index = tbl.top_capacity;
    
    Tundra::DynStk::clear(&tbl.available_cellar_indexes);
}

/**
 * @brief Adds a a pair to the Table, where `value` is associated to the hash 
 * of `key`.
 *
 * If the key/value pair already exists in the tbl, the value is replaced with
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
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool add(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, const key_type &key,
    const value_type &value) 
{
    return Tundra::HshTbl::Internal::underlying_add(tbl, 
        key, value, hash_func(key));
}

/**
 * @brief Removes the key/value pair associated with the given key from the 
 * Table. Returns true if the erasure was successful.
 *
 * @param tbl Pointer to the Table.
 * @param key Read-only pointer to the key use to look up the pair to erase.
 *
 * @return bool True if successful.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool erase(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, const key_type &key)
{
    Tundra::HshTbl::Internal::Entry<key_type, value_type> *table_data = 
        tbl.data;

    Tundra::HshTbl::Internal::Entry<key_type, value_type> *parsed_entry = 
        &table_data[hash_func(key) % tbl.top_capacity];

    Tundra::int64 parsed_entry_status = parsed_entry->status;

    // If there is no Entry at the hashed index.
    if(parsed_entry_status <= -2) { return false; }

    // -- Check the first Entry, since it is a special case in which it is known
    // that there are no previous entries that point to it that need updated if 
    // it is removed. --

    // If the first Entry is the key we're looking for.
    if(cmp_func(parsed_entry->key, key))
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
        Tundra::DynStk::push(tbl.available_cellar_indexes,
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
        if(!cmp_func(parsed_entry->key, key)) { continue; }

        // -- This is the key we're looking for. -- 

        // Add the index of the cellar entry that is being removed to the stack
        // of available cellar indexes. The previous Entry's status is used here
        // since it contains the index of the currently parsed Entry.
        Tundra::DynStk::push(tbl.available_cellar_indexes, 
            (Tundra::int64)previous_entry->status);
            
        // Update the previous Entry in the chain to point to what this Entry
        // is pointing to, since we're removing this Entry.
        // tbl.data[previous_entry_index].status =
        //     tbl.data[entry_index].status;
        previous_entry->status = parsed_entry->status;

        // Set the status of the Entry to -2 to mark it as empty, effectively
        // "removing" it from the Table. #TODO: Maybe we don't need this?
        // tbl.data[entry_index].status = -2;
        
        return true;
    }

    // We've iterated over the entire chain, the key was not found.
    return false;
}

/*
 * @brief Checks if there is an Entry in the Table with the passed `key`, 
 * returning true if there is.
 *
 * @param tbl Read-only pointer to the tbl.
 * @param key Read-only pointer to the key to look for.
 *
 * @return bool True if there is an Entry in the Table with the key.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline bool contains(const Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, const key_type &key)
{
    return Tundra::HshTbl::Internal::get_value_of_key(tbl, key) != nullptr;
}

/**
 * @brief Returns a pointer to the value tied to `key` in the tbl, nullptr if the
 * key/value pair does not exist.
 *
 * @param tbl Read-only pointer to the Table
 * @param key Read-only pointer to the key to find the value of.
 *
 * @return value_type* Pointer to the value associated with `key`, or nullptr if 
 * the key/value pair doesn't exist.
 */
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline value_type& at(Tundra::HshTbl::HashTable<key_type, value_type, 
    hash_func, cmp_func> &tbl, const key_type &key)
{
    value_type *val = Tundra::HshTbl::Internal::get_value_of_key(tbl, key);

    if(val != nullptr) { return *val; }

    TUNDRA_FATAL("Requested for value of key, but key was not found.");
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type&),
    bool (*cmp_func)(const key_type&, const key_type&)
>
inline const value_type& at(const Tundra::HshTbl::HashTable<key_type, 
    value_type, hash_func, cmp_func> &tbl, const key_type &key)
{
    value_type *val = Tundra::HshTbl::Internal::get_value_of_key(tbl, key);

    if(val != nullptr) { return *val; }

    TUNDRA_FATAL("Requested for value of key, but key was not found.");
}

// /**
//  * @brief Returns a pointer to the value tied to `key` in the tbl, nullptr if the
//  * key/value pair does not exist.
//  *
//  * @param tbl Read-only pointer to the Table
//  * @param key The key to find the value of.
//  *
//  * @return value_type* Pointer to the value associated with `key`, or nullptr if 
//  * the key/value pair doesn't exist.
//  */
// template
// <
//     typename key_type, typename value_type, 
//     Tundra::uint64 (*hash_func)(const key_type&),
//     bool (*cmp_func)(const key_type&, const key_type&)
// >
// inline value_type* at(const Tundra::HshTbl::HashTable<key_type, value_type, 
//     hash_func, cmp_func> &tbl, const key_type key)
// {
//     return Tundra::HshTbl::Internal::get_value_of_key(tbl, &key);
// }

} // namespace Tundra::HshTbl
