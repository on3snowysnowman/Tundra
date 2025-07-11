

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
#define TUNDRA_HSHTBL_CMPFUNC(a, b) Tundra_Str_compare(a, b)

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

// The ratio between used/capacity in the top of a HashTable, where if exceeded
// will be expanded and rehashed.
#define TUNDRA_HSHTBL_TABLETOP_LIMIT 0.7f

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
    // If this entry is 'active' inside the HashTable, and contains valid data. 
    bool is_active;

    // The key of this entry.
    TUNDRA_HSHTBL_KEYTYPE key;

    // The value of this entry.
    TUNDRA_HSHTBL_VALUETYPE value;

    // The index inside the Cellar of the entry that collided with this one.
    // Storing the index in this way allows us to create "chains" of entries
    // that share the same hash but NOT the same value, where each entry
    // points to the next one allowing quick parsing of chains. If this value is
    // -1, there is no next entry, and this entry marks the end of a chain.  
    int64_t collided_entry_index;

    // Computed hash of this entry.
    uint64_t hash;

} TUNDRA_HSHTBL_ENTRYSTRUCT_SIG; // HashEntry


/**
 * @brief Expanding container for storing key-value pairs.
 * 
 *  * Internal variables are read-only.
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
 
    // The largest index inside the cellar where an entry has been placed.
    uint64_t greatest_cellar_index;

    // Contains indexes inside of the cellar that been freed by an entry being
    // removed, and can be used.
    Tundra_DynamicStackUInt64 available_cellar_indexes; 

} TUNDRA_HSHTBL_TBLSTRUCT_SIG; // HashTable


// Private Methods -------------------------------------------------------------


// Forward declaration. 
void TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table);

/**
 * @brief Traverses the collision chain starting from the given entry to find 
 * the last entry in the chain. Calculates and returns the appropriate index in 
 * the cellar to place a new entry, and updates the last entry in the chain to 
 * point to this new index.
 *
 * @param table HashTable to analyze.
 * @param entry Entry where the collision occurred.
 * @return The index in the cellar where the new entry should be placed.
 */
uint64_t TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, uint64_t collided_index, 
    const TUNDRA_HSHTBL_KEYTYPE *key, const TUNDRA_HSHTBL_VALUETYPE *value,
    const uint64_t hash)
{
    // Find the last entry in the chain by looping until a -1 is found for a 
    // next placed index.
    while(table->data[collided_index].collided_entry_index != -1) 
        collided_index = table->data[collided_index].collided_entry_index;

    // Collided index now points to the last value in the chain. 
    
    uint64_t available_index;

    // If there are any recycled slots in the cellar.
    if(!Tundra_DynStkUInt64_is_empty(&table->available_cellar_indexes))
    {
        available_index = *Tundra_DynStkUInt64_front(
            &table->available_cellar_indexes);
        Tundra_DynStkUInt64_pop(&table->available_cellar_indexes);
    }

    // No recycled slots in the cellar, place after the greatest added cellar 
    // index.
    else
    {
        available_index = ++table->greatest_cellar_index;            
    }

    // Update the last entry in the chain to reflect that this position is a 
    // new addition to the entry chain that share the same hash.
    table->data[collided_index].collided_entry_index = (int64_t)available_index;

    table->data[available_index].collided_entry_index = -1;
    table->data[available_index].hash = hash;
    table->data[available_index].

    // If the cellar has reached its capacity.
    if(table->greatest_cellar_index == table->cellar_capacity - 1)
    {
        TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(table);
    }

    return 0;
}

void TUNDRA_HSHTBL_INTFUNC_SIG(_transfer_entry)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *new_table, 
    const TUNDRA_HSHTBL_ENTRYSTRUCT_SIG *entry)
{
    // The limit of the top has been reached.
    if(new_table->num_entries_top / (1.0f * new_table->top_capacity) >=
        TUNDRA_HSHTBL_TABLETOP_LIMIT) 
    { TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(new_table); }

    uint64_t index_into_top = entry->hash % new_table->top_capacity;

    if(!new_table->data[index_into_top].is_active)
    {
        new_table->data[index_into_top].value = entry->value;
        new_table->data[index_into_top].hash = entry->hash;
        new_table->data[index_into_top].collided_entry_index = -1;
        new_table->data[index_into_top].is_active = true;
        return;
    }

    TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)(new_table, index_into_top,
        &entry->key, &entry->value, entry->hash);
}

/**
 * @brief Resizes, reallocates and rehashes the HashTable to double its size.
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
    // new_table.num_entries_cellar = 0;
    new_table.top_capacity = 0.7f * new_total_capacity;
    new_table.cellar_capacity = new_total_capacity - new_table.top_capacity;
    new_table.greatest_cellar_index = 0;

    // Malloc heap for the new table.
    new_table.data = (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)malloc(new_total_capacity * 
        sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG));
       
    // Iterate through each spot in the top of the table.
    for(uint64_t i = 0; i < table->top_capacity; ++i)
    {
        if(!table->data[i].is_active) continue;

        TUNDRA_HSHTBL_INTFUNC_SIG(_transfer_entry)(&new_table,
            &table->data[i]);
    }

    free(table->data);
    *table = new_table;
}


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes the HashTable, allocating initial memory and setting 
 * internal components.
 * 
 * @param table Table to initialize.
 */
inline void TUNDRA_HSHTBL_FUNC_SIG(_init)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    table->data = (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)
        calloc(TUNDRA_HSHTBL_DFLT_INIT_ENTRIES, 
            sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG));

    // table->num_entries_top = 0;
    table->top_capacity = 0.7f * TUNDRA_HSHTBL_DFLT_INIT_ENTRIES;
    // table->num_entries_cellar = 0;
    table->cellar_capacity = TUNDRA_HSHTBL_DFLT_INIT_ENTRIES - 
        table->top_capacity;
    // table->greatest_cellar_index = 0;
    
    Tundra_DynStkUInt64_init(&table->available_cellar_indexes, 
        16);
}

/**
 * @brief Adds a pair to the table, where `value` is associated to the 
 * hash of `key`.
 * 
 * @param key Key of the pair, used as a lookup for `value`
 * @param value Value of the pair, retrieved when `key` is looked up.
 */
void TUNDRA_HSHTBL_FUNC_SIG(_add)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table,
    const TUNDRA_HSHTBL_KEYTYPE *key, const TUNDRA_HSHTBL_VALUETYPE *value)
{
    // The limit of the top has been reached.
    if(table->num_entries_top / (1.0f * table->top_capacity) >=
        TUNDRA_HSHTBL_TABLETOP_LIMIT) TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(table);

    uint64_t hash = TUNDRA_HSHTBL_HASHFUNC(*key);

    uint64_t index_into_top = hash % table->top_capacity;

    // There is no entry at the computed index. Simply place this value here.
    if(!table->data[index_into_top].is_active)
    {
        table->data[index_into_top].key = *key;
        table->data[index_into_top].value = *value;
        table->data[index_into_top].hash = hash;
        table->data[index_into_top].collided_entry_index = -1;
        table->data[index_into_top].is_active = true;
        return;
    }

    // We have an active entry there already. Check if its key matches the
    // same key as we are attempting to add.

    // Compare the key of the target entry to the key being added.
    if(!TUNDRA_HSHTBL_CMPFUNC(table->data[index_into_top].key, *key))
    {
        // Entry key does not match, handle the collision.
        TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)(table, index_into_top, 
            key, value, hash);
    }

    // This Entry shares the key being added, simply update its value.
    table->data[index_into_top].value = *value;
}

inline void TUNDRA_HSHTBL_FUNC_SIG(_deconstruct)(
    TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    free(table->data);
    table->data = NULL;
    Tundra_DynStkUInt64_deconstruct(&table->available_cellar_indexes);
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

#undef TUNDRA_HSHTBL_TBLSTRUCT_SIG
#undef TUNDRA_HSHTBL_ENTRYSTRUCT_SIG
#undef TUNDRA_HSHTBL_FUNC_SIG
#undef TUNDRA_HSHTBL_INTFUNC_SIG
#undef TUNDRA_HSHTBL_HASHFUNC
#undef TUNDRA_HSHTBL_DFLT_INIT_ENTRIES
#undef TUNDRA_HSHTBL_TABLETOP_LIMIT