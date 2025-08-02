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


namespace Tundra::HshTbl
{

namespace Internal
{

// Default memory alignment.    
constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

template<typename key, typename value>
struct Entry
{
    // The key of this entry. 
    key node_key;

    // The value of this entry.
    value node_value;

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

template<typename key, typename value, 
    Tundra::uint64 (*hash_func)(const key*) = Tundra::hash<key>, 
    Tundra::uint8 alignment = Tundra::HshTbl::Internal::DEFAULT_ALIGNMENT>
struct HashTable 
{
    // Full array of all Entrys.
    Tundra::HshTbl::Internal::Entry<key, value> *data;

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

template<typename key, typename value, Tundra::uint64 (*hash_func)(const key*), 
    Tundra::uint8 alignment>
void underlying_init(
    Tundra::HshTbl::HashTable<key, value, hash_func, alignment> *table,
    Tundra::uint64 init_capacity)
{
    table->data = (Tundra::HshTbl::Internal::Entry<key, value>*)
        Tundra::alloc_aligned<alignment>(init_capacity * 
            sizeof(Tundra::HshTbl::Internal::Entry<key, value>));
        
    
}

} // namespace Internal


// Public ---------------------------------------------------------------------




} // namespace Tundra::HshTbl
