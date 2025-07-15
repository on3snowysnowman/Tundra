

#ifndef TUNDRA_HGUARD_HASHTABLE_H
#define TUNDRA_HGUARD_HASHTABLE_H

#include <stdint.h>

#include "tundra/tundra_utils/MacroUtils.h"
#include "tundra/tundra_utils/Hash.h"

#endif // TUNDRA_HGUARD_HASHTABLE_H

// Macros ----------------------------------------------------------------------

// Key type the HashTable stores.
#ifndef TUNDRA_HSHTBL_KEYTYPE
#define TUNDRA_HSHTBL_KEYTYPE_MANUALLY_SET
#define TUNDRA_HSHTBL_KEYTYPE int
#endif

// If the hash function is not defined, check if the set key type is a primitive
// type which has built in hashing support. Otherwise, the key type is a user 
// defined struct and needs a custom hash function set.
#ifndef TUNDRA_HSHTBL_HASHFUNC
#define TUNDRA_HSHTBL_HASHFUNC_MANUALLY_SET
#if TUNDRA_HSHTBL_KEYTYPE == uint8_t
#define TUNDRA_HSHTBL_HASHFUNC(item) Tundra_hash_uint8(item)

#elif TUNDRA_HSHTBL_KEYTYPE == uint16_t
#define TUNDRA_HSHTBL_HASHFUNC(item) uint64_t Tundra_hash_uint16(item)

#elif TUNDRA_HSHTBL_KEYTYPE == uint32_t
#define TUNDRA_HSHTBL_HASHFUNC(item) Tundra_hash_uint32(item)

#elif TUNDRA_HSHTBL_KEYTYPE == uint64_t
#define TUNDRA_HSHTBL_HASHFUNC(item) Tundra_hash_uint64(item)

#else 
#error A hash function must be provided for a non primitive or Tundra_String \
       type!
#define TUNDRA_HSHTBL_HASHFUNC(item) TUNDRA_NO_SPECIFIED_HASHFUNC

#endif 
#endif // #ifndef TUNDRA_HSHTBL_HASHFUNC

// If the comparison function is not defined, check if the set key type is a 
// primitive type which has built in comparison support. Otherwise, the key type
// is a user defined struct and needs a custom comparison function set.
#ifndef TUNDRA_HSHTBL_CMPFUNC
#define TUNDRA_HSHTBL_CMPFUNC_MANUALLY_SET

#if TUNDRA_HSHTBL_KEYTYPE == uint8_t
#define TUNDRA_HSHTBL_CMPFUNC(a, b) ((a) == (b)) 

#elif TUNDRA_HSHTBL_KEYTYPE == uint16_t
#define TUNDRA_HSHTBL_CMPFUNC(a, b) ((a) == (b)) 

#elif TUNDRA_HSHTBL_KEYTYPE == uint32_t
#define TUNDRA_HSHTBL_CMPFUNC(a, b) ((a) == (b)) 

#elif TUNDRA_HSHTBL_KEYTYPE == uint64_t
#define TUNDRA_HSHTBL_CMPFUNC(a, b) ((a) == (b)) 

#elif TUNDRA_HSHTBL_KEYTYPE == Tundra_String
#include "tundra/tundra_utils/String.h"
#define TUNDRA_HSHTBL_CMPFUNC(a, b) Tundra_Str_compare(&a, &b)

#else 
#error A compare function must be provided for a non primitive or Tundra_String\
       type!
#define TUNDRA_HSHTBL_HASHFUNC(item) TUNDRA_NO_SPECIFIED_CMPFUNC

#endif
#endif // #ifndef TUNDRA_HSHTBL_CMPFUNC

// Value type the of the HashTable.
#ifndef TUNDRA_HSHTBL_VALUETYPE
#define TUNDRA_HSHTBL_VALUETYPE_MANUALLY_SET
#define TUNDRA_HSHTBL_VALUETYPE int
#endif

// Name identifier for the specific type instance of the HashTable
#ifndef TUNDRA_HSHTBL_NAME
#define TUNDRA_HSHTBL_NAME_MANUALLY_SET
#define TUNDRA_HSHTBL_NAME NO_SPECIFIED_NAME
#endif

// Full signature of the HashTable struct.
#define TUNDRA_HSHTBL_TBLSTRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_HashTable, TUNDRA_HSHTBL_NAME)

// Full signature of the HashEntry struct.
#define TUNDRA_HSHTBL_ENTRYSTRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_HashEntry, TUNDRA_HSHTBL_NAME)

// Function signature for a HashTable of a given type.
#define TUNDRA_HSHTBL_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_HshTbl, TUNDRA_HSHTBL_NAME, func_name)

// Internal function signature for a HashTable of a given type
#define TUNDRA_HSHTBL_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(InternalTundra_HshTbl, TUNDRA_HSHTBL_NAME , func_name)

// Default initial entries that will be allocated for a new HashTable
#define TUNDRA_HSHTBL_DFLT_INIT_ENTRIES 16

#ifdef TUNDRA_BENCHMARKING

// Use modifiable values instead of macros for benchmarking to modify.
static float TUNDRA_HSHTBL_TOP_LIMIT = 0.8f;
static float TUNDRA_HSHTBL_TOP_PROPORTION = 0.7f;

#else

// The ratio between used/capacity in the top of a HashTable, where if exceeded
// will be expanded and rehashed.
#define TUNDRA_HSHTBL_TOP_LIMIT 0.7f

// The proportion of the total table capacity that will be allocated for the top
// of the table, where the remaining space is reserved for the cellar.
#define TUNDRA_HSHTBL_TOP_PROPORTION 0.8f

#endif

// Tundra Library Container Definitions ----------------------------------------

#ifndef TUNDRA_DYNAMICSTACK_UINT64
#define TUNDRA_DYNAMICSTACK_UINT64
#define TUNDRA_DYNSTK_TYPE uint64_t
#define TUNDRA_DYNSTK_NAME UInt64
#include "tundra/tundra_utils/DynamicStack.h"
#undef TUNDRA_DYNSTK_TYPE
#undef TUNDRA_DYNSTK_NAME
#endif

// -----------------------------------------------------------------------------


/**
 * @brief Contains data for a single entry in the HashTable.
 *
 * Users do not need to interact with this container, it is used internally by 
 * the Tundra_HashTable.
 */
typedef struct
{
    // The key of this entry.
    TUNDRA_HSHTBL_KEYTYPE key;

    // The value of this entry.
    TUNDRA_HSHTBL_VALUETYPE value;

    // Tracks 1 of 3 states the entry can hold. If this int is -2, the Entry is
    // not initialized/used and a key/value pair can be placed here. If -1, the
    // This entry is valid and holds an added key/value pair, and no other 
    // entries have been added with a different key but the same hash. If 
    // greater than -1, this int stores the index inside the cellar of the 
    // entry that collided with this one and had to be stored in the cellar, 
    // creating a "pointer" to it, establishing a chain.
    int64_t status;

    // Computed hash of this entry.
    uint64_t hash;

} TUNDRA_HSHTBL_ENTRYSTRUCT_SIG; // HashEntry


/**
 * @brief Expanding container for storing key-value pairs.
 * 
 * Internal variables are read-only.
 */
typedef struct
{
    TUNDRA_HSHTBL_ENTRYSTRUCT_SIG* data;

    // Number of entries added to the "top" of the HashTable.
    uint64_t num_entries_top; 

    // Capacity in HashEntrys of the "top" of the HashTable 
    uint64_t top_capacity;

    // Capacity in HashEntrys of the "cellar" of the HashTable.
    uint64_t cellar_capacity;
 
    // The index inside the cellar of the next empty position, assuming the 
    // `available_cellar_indexes` stack is empty.
    uint64_t next_available_cellar_index;

    // Contains indexes inside of the cellar that been freed by an entry being
    // removed, and can be used.
    Tundra_DynamicStackUInt64 available_cellar_indexes; 

} TUNDRA_HSHTBL_TBLSTRUCT_SIG; // HashTable


// Private Methods -------------------------------------------------------------

// Forward declarations. 
void TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table);
void TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key,
    const TUNDRA_HSHTBL_VALUETYPE *value, const uint64_t hash);

/**
 * @brief Underlying initialization method, initializes the table and allocates
 * `init_capacity` elements for the hash lookup array. 
 * 
 * This is an internal function, users should disregard it.
 * 
 * @param table Table to initialize.
 * @param init_capacity Initial capacity of entries to allocate for.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_init)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, uint64_t init_capacity)
{
    table->data = (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)malloc(
        init_capacity * sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG));

    // Iterate through each entry and set its status to -2, to flag it as ab
    // empty entry.
    for(uint64_t i = 0; i < init_capacity; ++i)
    {
        // Don't need to worry about setting any other values of the entries 
        // since they will be overwritten when a valid entry is placed here.
        table->data[i].status = -2;
    }

    table->num_entries_top = 0;
    table->top_capacity = TUNDRA_HSHTBL_TOP_PROPORTION * init_capacity;
    table->cellar_capacity = init_capacity - table->top_capacity;
    table->next_available_cellar_index = table->top_capacity;
    
    Tundra_DynStkUInt64_init(&table->available_cellar_indexes, 
        table->cellar_capacity);
}

/**
 * @brief Handles a collision when an entry is attempted to be placed at a 
 * position with the same hashed index, but different key.
 * 
 * If the top limit has not been reached and the cellar is not full, the first
 * available position in the cellar is found and the collided entry is placed 
 * there, and the entry's `status` at `collided_index`'s is updated to reflect 
 * the new position that the new entry was placed at.
 * 
 * If the cellar is full or the top limit has been reached, the table is 
 * resized and this collided entry is placed in it.
 * 
 * This is an internal function, users should disregard it.
 *
 * @param table HashTable to analyze.
 * @param entry Entry where the collision occurred.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, uint64_t collided_index, 
    const TUNDRA_HSHTBL_KEYTYPE *key, const TUNDRA_HSHTBL_VALUETYPE *value,
    const uint64_t hash)
{
    if(table->num_entries_top / (1.0f * table->top_capacity) >=
            TUNDRA_HSHTBL_TOP_LIMIT ||
        table->next_available_cellar_index >= table->cellar_capacity
            + table->top_capacity)
    {
        TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(table);
        TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(table, key, value, hash);
        return;
    }

    // Find the last entry in the chain by looping until a -1 is found for an 
    // entry's status, which flags there are no further nodes in the chain.
    while(table->data[collided_index].status > -1) 
        collided_index = table->data[collided_index].status;

    // Collided index now points to the last entry in the chain. 
    
    uint64_t available_index;

    // If there are any recycled slots in the cellar.
    if(!Tundra_DynStkUInt64_is_empty(&table->available_cellar_indexes))
    {
        available_index = *Tundra_DynStkUInt64_front(
            &table->available_cellar_indexes);
        Tundra_DynStkUInt64_pop(&table->available_cellar_indexes);
    }

    // No recycled slots in the cellar, place at the next available cellar 
    // index.
    else
    {
        available_index = table->next_available_cellar_index++;            
    }

    // Update the last entry in the chain to reflect that this position is a 
    // new addition to the entry chain that share the same hash.
    table->data[collided_index].status = available_index;

    table->data[available_index].status = -1;
    table->data[available_index].hash = hash;
    table->data[available_index].key = *key;
    table->data[available_index].value = *value;
}

/**
 * @brief Underlying method for adding a key/value pair to the table.
 * 
 * @param table Table to modify.
 * @param key Key of the new pair.
 * @param value Value of the new pair.
 * @param hash Hash of the key.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key,
    const TUNDRA_HSHTBL_VALUETYPE *value, const uint64_t hash)
{
    uint64_t index_into_top = hash % table->top_capacity;

    // There is no entry at the computed index. Simply place this value here.
    if(table->data[index_into_top].status <= -2)
    {
        table->data[index_into_top].key = *key;
        table->data[index_into_top].value = *value;
        table->data[index_into_top].hash = hash;
        table->data[index_into_top].status = -1;

        ++table->num_entries_top;
        return;
    }

    // We have an active entry there already. Check if its key matches the
    // same key as we are attempting to add.

    // If the key at the initial entry is the same as our added key, simply 
    // update its value
    if(TUNDRA_HSHTBL_CMPFUNC(table->data[index_into_top].key, *key))
    {
        // This Entry shares the key being added, simply update its value.
        table->data[index_into_top].value = *value;
        return;
    }

    // Initial entry key doesn't match, so we have a collision. Check if 

    // Entry key does not match, handle the collision.
    TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)(table, index_into_top, 
        key, value, hash);
}

/**
 * @brief Transfers a node chain starting at the passed initial index to a new
 * table.
 * 
 * If the chain is only one entry long and only exists in the top of the table,
 * only the single entry will be moved. 
 * 
 * This is an internal function, users should disregard it. 
 * 
 * @param old_table Table to move chain from.
 * @param new_table Table to move chain to.
 * @param initial_index Index of the first entry in the chain.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_transfer_entry_chain)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *old_table, 
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *new_table, 
    int64_t initial_index)
{
    // Add the initial entry.
    TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(new_table, 
            &old_table->data[initial_index].key, 
            &old_table->data[initial_index].value,
            old_table->data[initial_index].hash);

    // Iterate through each entry in the chain and add it to the new table.
    while (old_table->data[initial_index].status > -1)
    {
        // Update the index to point to the next entry in the chain.
        initial_index = old_table->data[initial_index].status;

        TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(new_table, 
            &old_table->data[initial_index].key, 
            &old_table->data[initial_index].value,
            old_table->data[initial_index].hash);
    } 
}

/**
 * @brief Resizes, reallocates and rehashes the HashTable to double its size.
 * 
 * This is an internal function, users should disregard it. 
 * 
 * @param table Table to resize.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    TUNDRA_HSHTBL_TBLSTRUCT_SIG new_table;

    // Double the old capacity.
    uint64_t new_total_capacity = 
        (table->top_capacity + table->cellar_capacity) * 2;

    // Initialize default values for new table.
    new_table.num_entries_top = 0;
    new_table.top_capacity = TUNDRA_HSHTBL_TOP_PROPORTION * new_total_capacity;
    new_table.cellar_capacity = new_total_capacity - new_table.top_capacity;
    new_table.next_available_cellar_index = new_table.top_capacity;

    // Malloc heap for the new table.
    new_table.data = (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)malloc(new_total_capacity * 
        sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG));

    // Iterate through each entry in the new table and set its status to empty.
    for(uint64_t i = 0; i < new_total_capacity; ++i)
    {
        new_table.data[i].status = -2;
    }

    Tundra_DynStkUInt64_clear(&table->available_cellar_indexes);
    new_table.available_cellar_indexes = table->available_cellar_indexes;
       
    // Iterate through each spot in the top of the table.
    for(uint64_t i = 0; i < table->top_capacity; ++i)
    {
        // If this entry is empty.
        if(table->data[i].status <= -2) 
        {
            // Set the status of the entry in the new table to empty.
            new_table.data[i].status = -2;
            continue;
        };

        // Transfer this entry and any other in its chain to the new table.
        TUNDRA_HSHTBL_INTFUNC_SIG(_transfer_entry_chain)(table, &new_table,
            i);
    }

    free(table->data);
    *table = new_table;
}

/**
 * @brief Returns the value associated with a key, NULL if there is no such key.
 * 
 * @param table Table to analyze.
 * @param key Key to find value of.
 */
TUNDRA_HSHTBL_VALUETYPE* TUNDRA_HSHTBL_INTFUNC_SIG(_get_key_value)(
    const TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key)
{
    uint64_t index_into_top = TUNDRA_HSHTBL_HASHFUNC(*key) % 
        table->top_capacity;

    // The entry at the computed hash index is empty.
    if(table->data[index_into_top].status <= -2) return NULL;

    // This entry is not empty, check initial entry.

    // If the initial entry is the key we're searching for.
    if(TUNDRA_HSHTBL_CMPFUNC(table->data[index_into_top].key, *key)) 
        return &table->data[index_into_top].value;;

    // The entry at the first position inside the top is not the key we're 
    // searching for. Iterate over the chain of keys that share the same hash,
    // checking each one until we've either found a match for our key or have
    // reached the end of the chain.
    while(table->data[index_into_top].status > -1)
    {
        index_into_top = table->data[index_into_top].status;

        // If we've found our key.
        if(TUNDRA_HSHTBL_CMPFUNC(table->data[index_into_top].key, *key))
            return &table->data[index_into_top].value;
    }

    // If we've reached this point, we're at the end of the chain and the 
    // key hasn't been found.
    return NULL;
}

// Public Methods --------------------------------------------------------------

/**
 * @brief Removes the key/value pair associated with the given key from the 
 * table.
 * 
 * @param table Table to erase from.
 * @param key Key whose entry should be removed.
 */
static bool TUNDRA_HSHTBL_FUNC_SIG(_erase)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table,
    TUNDRA_HSHTBL_KEYTYPE *key)
{
    // First check if there is an entry at the computed hash of the key.
    uint64_t entry_index = TUNDRA_HSHTBL_HASHFUNC(*key) % 
        table->top_capacity;

    // There is no entry at the hashed index.
    if(table->data[entry_index].status <= -2) return false;

    // Check the first entry, since it is a special case in which it is known 
    // there are no previous entries that point to it that need to be updated 
    // if it is removed.

    // If the first entry is th key we're looking for.
    if(TUNDRA_HSHTBL_CMPFUNC(table->data[entry_index].key, *key))
    {
        // If this entry is not pointing to any entry in the cellar, we can just
        // remove it.
        if(table->data[entry_index].status <= -1)
        {
            table->data[entry_index].status = -2;
            return true;
        }

        // The first entry is pointing to an entry in the cellar, so we need to 
        // move that entry to this position inside the top since we're removing
        // the entry from the top.
        table->data[entry_index] = 
            table->data[table->data[entry_index].status];

        // Add the index of the cellar entry that is being moved to the 
        // stack of available cellar indexes since it's being moved.
        Tundra_DynStkUInt64_push(&table->available_cellar_indexes, 
            (uint64_t*)&table->data[entry_index].status);
        return true;
    }

    // If we're at this point, then the first entry in the top of the table was
    // not the key we were looking for, and we are now iterating over the entry 
    // chain of collided entries.
    
    // Store the previous entry so if we remove an entry, we can update the 
    // previous one to point to the entry that the removed entry points to, like
    // removing a node in a linked list.
    uint64_t previous_entry_index = entry_index;
    
    // Place the entry index at the next entry in the chain.
    entry_index = table->data[entry_index].status;

    while (table->data[entry_index].status > -1)
    {
        // If this is the key we're looking for.
        if(TUNDRA_HSHTBL_CMPFUNC(table->data[entry_index].key, *key))
        {
            // Update the previous entry in the chain to point to what this 
            // entry is pointing to, since we're removing this entry.
            table->data[previous_entry_index].status =
                    table->data[entry_index].status;

            // Set the status of this entry to -2 to mark it as empty, 
            // effectively "removing" it from the table.
            table->data[entry_index].status = -2;

            // Add the index of the cellar entry that is being removed to the 
            // stack of available cellar indexes since it's being removed.
            Tundra_DynStkUInt64_push(&table->available_cellar_indexes, 
                &entry_index);
            
            return true;
        }

        previous_entry_index = entry_index;

        // Place the entry index at the next entry in the chain.
        entry_index = table->data[entry_index].status;
    } 
        
    // We've iterated over the entire entry chain, and the key was not found.
    return false;
}

/**
 * @brief Returns true if there is an entry in the table with `key`. 
 * 
 * @param key Key to check for.
 */
static inline bool TUNDRA_HSHTBL_FUNC_SIG(_contains)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key)
{
    return TUNDRA_HSHTBL_INTFUNC_SIG(_get_key_value) != NULL;
}

/**
 * @brief Initializes the HashTable, allocating initial memory and setting 
 * internal components.
 * 
 * @param table Table to initialize.
 */
static inline void TUNDRA_HSHTBL_FUNC_SIG(_init)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_init)(table, 
        TUNDRA_HSHTBL_DFLT_INIT_ENTRIES);
}

/**
 * @brief Initializes the HashTable, allocating initial memory for 
 * `init_capacity` elements and setting internal components.
 * 
 * @param table Table to initialize.
 * @param init_capacity Initial capacity of elements to allocate for the 
 *    internal hash lookup array.
 */
static inline void TUNDRA_HSHTBL_FUNC_SIG(_init_with_capacity)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, uint64_t init_capacity)
{
    TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_init)(table, init_capacity);
}

/**
 * @brief Handles deletion of heap allocated memory for this hash table.
 * 
 * The table can be safely discarded after this method is called.
 * 
 * @param table Table to deconstruct.
 */
static inline void TUNDRA_HSHTBL_FUNC_SIG(_deconstruct)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    Tundra_DynStkUInt64_deconstruct(&table->available_cellar_indexes);
    free(table->data);
    table->data = NULL;
}

/**
 * @brief Adds a pair to the table, where `value` is associated to the 
 * hash of `key`.
 * 
 * If the key/value pair already exists in the table, the value is replaced with
 * `value`.
 * 
 * @param key Key of the pair, used as a lookup for `value`
 * @param value Value of the pair, retrieved when `key` is looked up.
 */
static inline void TUNDRA_HSHTBL_FUNC_SIG(_add)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key, 
    const TUNDRA_HSHTBL_VALUETYPE *value)
{
    TUNDRA_HSHTBL_INTFUNC_SIG(_underlying_add)(table, key, value, 
        TUNDRA_HSHTBL_HASHFUNC(*key));
}

/**
 * @brief Returns a pointer to the value tied to `key` in the table, 
 * NULL if the key/value pair does not exist.
 * 
 * @param table Table to analyze.
 * @param key Key to find value of.
 */
static inline TUNDRA_HSHTBL_VALUETYPE* TUNDRA_HSHTBL_FUNC_SIG(_at)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, const TUNDRA_HSHTBL_KEYTYPE *key)
{
    return TUNDRA_HSHTBL_INTFUNC_SIG(_get_key_value)(table, key);
}

#ifdef TUNDRA_HSHTBL_KEYTYPE_MANUALLY_SET
#undef TUNDRA_HSHTBL_KEYTYPE_MANUALLY_SET
#undef TUNDRA_HSHTBL_KEYTYPE
#endif

#ifdef TUNDRA_HSHTBL_VALUETYPE_MANUALLY_SET
#undef TUNDRA_HSHTBL_VALUETYPE_MANUALLY_SET
#undef TUNDRA_HSHTBL_VALUETYPE
#endif

#ifdef TUNDRA_HSHTBL_NAME_MANUALLY_SET
#undef TUNDRA_HSHTBL_NAME_MANUALLY_SET
#undef TUNDRA_HSHTBL_NAME
#endif

#ifdef TUNDRA_HSHTBL_HASHFUNC_MANUALLY_SET
#undef TUNDRA_HSHTBL_HASHFUNC_MANUALLY_SET
#undef TUNDRA_HSHTBL_HASHFUNC
#endif

#ifdef TUNDRA_HSHTBL_CMPFUNC_MANUALLY_SET
#undef TUNDRA_HSHTBL_CMPFUNC_MANUALLY_SET
#undef TUNDRA_HSHTBL_CMPFUNC
#endif

#undef TUNDRA_HSHTBL_TBLSTRUCT_SIG
#undef TUNDRA_HSHTBL_ENTRYSTRUCT_SIG
#undef TUNDRA_HSHTBL_FUNC_SIG
#undef TUNDRA_HSHTBL_INTFUNC_SIG
#undef TUNDRA_HSHTBL_DFLT_INIT_ENTRIES
#undef TUNDRA_HSHTBL_TOP_LIMIT