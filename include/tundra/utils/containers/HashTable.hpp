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
    // The key of this entry. 
    key_type key;

    // The value of this entry.
    value_type value;

    // Tracks 1 of 3 states the entry can hold. If this int is -2, the Entry is
    // not initialized/used and a key/value pair can be placed here. If -1, the
    // This entry is valid and holds an added key/value pair, and no other 
    // entries have been added with a different key but the same hash. If 
    // greater than -1, this int stores the index inside the cellar of the 
    // entry that collided with this one and had to be stored in the cellar, 
    // creating a "pointer" to it, establishing a chain.
    Tundra::int64 status;

    // Computed hash of this entry's key.
    Tundra::uint64 hash;
};


} // namespace Internal


// Containers ------------------------------------------------------------------

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*) = Tundra::hash<key_type>, 
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
    Tundra::DynStk::DynamicStack<Tundra::uint64, alignment> 
        available_cellar_indexes;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*), 
    Tundra::uint8 alignment
>
void resize(Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    alignment> *table)
{
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, alignment> 
        new_table;

    // Double the old capacity.
    Tundra::uint64 new_total_capacity = 
        (table->top_capacity + table->cellar_capacity) * 2;

    new_table.num_entries_top = 0;
    new_table->top_capacity = (new_total_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10;
    new_table.cellar_capacity = new_total_capacity - new_table.top_capacity;
    new_table.next_available_cellar_index = new_table.top_capacity;

    new_table.data = Tundra::alloc_aligned<alignment>(new_total_capacity *
        sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
        
    // Set all sets of 8 bytes in the array of Entrys to -2, since we want the 
    // `status` variable of each Entry to be -2, and the other members of each
    // entry will be overriden later when a valid Entry is placed there.
    Tundra::set_mem_8_bytes((void*)table->data, 
        (sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>) * 
            new_total_capacity) / 8, -2);
}

template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*), 
    Tundra::uint8 alignment
>
void underlying_init(Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    alignment> *table, Tundra::uint64 init_capacity)
{
    table->data = (Tundra::HshTbl::Internal::Entry<key_type, value_type>*)
        Tundra::alloc_aligned<alignment>(init_capacity * 
            sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>));
    
    table->num_entries_top = 0;
    table->top_capacity = (init_capacity * 
        Tundra::HshTbl::Internal::TOP_PROPORTION) / 10;
    table->cellar_capacity = init_capacity - table->top_capacity;
    table->next_available_cellar_index = table->top_capacity;

    // Set all sets of 8 bytes in the array of Entrys to -2, since we want the 
    // `status` variable of each Entry to be -2, and the other members of each
    // entry will be overriden later when a valid Entry is placed there.
    Tundra::set_mem_8_bytes((void*)table->data, 
        (sizeof(Tundra::HshTbl::Internal::Entry<key_type, value_type>) * 
            init_capacity) / 8, -2);
}

// Forward declaration.
template
<
    typename key_type, typename value_type, 
    Tundra::uint64 (*hash_func)(const key_type*), 
    Tundra::uint8 alignment
>
void handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    alignment> *table, Tundra::uint64 collided_index, 
    const key_type *key, const value_type *value, Tundra::uint64 hash);

template
<
    typename key_type, typename value_type,
    Tundra::uint64 (*hash_func)(const key_type*),
    Tundra::uint8 alignment
>
void underlying_add(Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    alignment> *table, const key_type *key, const value_type *value, 
    Tundra::uint64 hash)
{
    Tundra::uint64 index_into_top = hash % table->top_capacity;

    // There is no entry at the computed index. Place the Entry here.
    if(table->data[index_into_top].status <= -2)
    {
        table->data[index_into_top].key = *key;
        table->data[index_into_top].value = *value;
        table->data[index_into_top].hash = hash;
        table->data[index_into_top].status = -1;

        ++table->num_entries_top;
        return;
    }

    // We have an active Entry at the computed position. Check if its key 
    // matches the same key as the one we are adding.
    if(Tundra::is_matching_value(&table->data[index_into_top].key, key))
    {
        // This Entry shares the key being added, simply update its value.
        table->data[index_into_top].value = *value;
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
    Tundra::uint8 alignment
>
void handle_collision(
    Tundra::HshTbl::HashTable<key_type, value_type, hash_func, 
    alignment> *table, Tundra::uint64 collided_index, 
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
    
    // Find the last entry in the chain by looping until a -1 is found for an 
    // Entry's status, which flags there are no further nodes in the chain.
    while(table->data[collided_index].status > -1)
        { collided_index = table->data[collided_index].status; }
    
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
    table->data[collided_index].status = available_index;

    table->data[available_index].status = -1;
    table->data[available_index].hash = hash;
    table->data[available_index].key = *key;
    table->data[available_index].value = *value;
}

} // namespace Internal


// Public ---------------------------------------------------------------------




} // namespace Tundra::HshTbl
