

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

// If the hash function is not defined, iterate through primitive types which 
// have native hash functions provided. If the hash func is not defined and the
// type of key is not a primitive type, set an invalid hash function so the 
// compilation fails.
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
#error A hash function must be provided for a non primitive type!
#define TUNDRA_HSHTBL_HASHFUNC(item) TUNDRA_NO_SPECIFIED_HASHFUNC(item)

#endif 
#endif // #ifndef TUNDRA_HSHTBL_HASHFUNC

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

    // The value of this entry in the HashTable.
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

    // Number of entires added to the "cellar" of the HashTable.
    uint64_t num_entries_cellar;

    // Capacity in HashEntrys of the "cellar" of the HashTable.
    uint64_t cellar_capacity;
 
    // The largest index inside the cellar where an entry has been placed.
    uint64_t greatest_cellar_index;

    // Contains indexes inside of the cellar that been freed by an entry being
    // removed, and can be used.
    Tundra_DynamicStackUInt64 available_cellar_indexs; 

} TUNDRA_HSHTBL_TBLSTRUCT_SIG; // HashTable


// Private Methods -------------------------------------------------------------

/**
 * @brief Resizes, reallocates and rehashes the HashTable to double its size.
 * 
 * @param table Table to resize.
 */
void TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    TUNDRA_HSHTBL_ENTRYSTRUCT_SIG* new_data = 
        (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)
        malloc(
            (table->top_capacity + table->cellar_capacity) * 
            2 * sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG));
       
    // Iterate through each spot in the top of the table.
    for(uint64_t i = 0; i < table->top_capacity; ++i)
    {
        if(!table->data[i].is_active) continue;

        // Get index into the new array.
    }

    free(table->data);
    table->data = new_data;
}

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
uint64_t TUNDRA_HSHTBL_INTFUNC_SIG(_handle_collision)
    (TUNDRA_HSHTBL_TBLSTRUCT_SIG *table, TUNDRA_HSHTBL_ENTRYSTRUCT_SIG *entry)
{
    // Find the last entry in the chain by looping until a -1 is found for a 
    // next placed index.
    while(entry->collided_entry_index != -1) 
        entry = &table->data[table->top_capacity + entry->collided_entry_index];



    return 0;
}


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes the HashTable, allocating initial memory and setting 
 * internal components.
 * 
 * @param table Table to initialize.
 */
void TUNDRA_HSHTBL_FUNC_SIG(_init)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    table->data = (TUNDRA_HSHTBL_ENTRYSTRUCT_SIG*)
        malloc(sizeof(TUNDRA_HSHTBL_ENTRYSTRUCT_SIG) * 
            TUNDRA_HSHTBL_DFLT_INIT_ENTRIES);

    table->num_entries_top = 0;
    table->top_capacity = 0.7f * TUNDRA_HSHTBL_DFLT_INIT_ENTRIES;
    table->num_entries_cellar = 0;
    table->cellar_capacity = TUNDRA_HSHTBL_DFLT_INIT_ENTRIES - 
        table->top_capacity;
    
    Tundra_DynStkUInt64_init(&table->available_cellar_indexs, 
        16);
}

/**
 * @brief Adds a pair to the table, where `value` is associated to the 
 * hash of `key`.
 * 
 * @param key Key of the pair, used as a lookup for `value`
 * @param value Value of the pair, retrieved when `key` is looked up.
 */
void TUNDRA_HSHTBL_FUNC_SIG(_emplace)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table,
    const TUNDRA_HSHTBL_KEYTYPE *key, const TUNDRA_HSHTBL_VALUETYPE *value)
{
    // The limit of the top has been reached.
    if(table->num_entries_top / (1.0f * table->top_capacity) >=
        TUNDRA_HSHTBL_TABLETOP_LIMIT) TUNDRA_HSHTBL_INTFUNC_SIG(_resize)(table);

    uint64_t hash = TUNDRA_HSHTBL_HASHFUNC(*key);

    uint64_t index_into_top = 
        hash % table->top_capacity;

    // There is no entry at the computed index. Simply place this value here.
    if(!table->data[index_into_top].is_active)
    {
        table->data[index_into_top].value = *value;
        table->data[index_into_top].hash = hash;
        table->data[index_into_top].collided_entry_index = -1;
        table->data[index_into_top].is_active = true;
        return;
    }

    // We have an active value there already, so we need to handle a collision.


}

void TUNDRA_HSHTBL_FUNC_SIG(_deconstruct)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{
    free(table->data);
    table->data = NULL;
    Tundra_DynStkUInt64_deconstruct(&table->available_cellar_indexs);
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