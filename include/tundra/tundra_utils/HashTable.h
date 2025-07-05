

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

#ifndef TUNDRA_HSHTBL_HASHFUNC
#define TUNDRA_HSHTBL_HASHFUNC NO_SPECIFIED_HASHFUNC
#else

#if TUNDRA_HSHTBL_KEYTYPE == uint8_t
#define TUNDRA_HSHTBL_HASHFUNC Tundra_Hash_uint8
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
    // The value of this entry in the HashTable.
    TUNDRA_HSHTBL_VALUETYPE value;

    // The index inside the Cellar of the entry that collided with this one.
    // Storing the index in this way allows us to create "chains" of entries
    // that share the same hash but NOT the same value, where each entry
    // points to the next one allowing quick parsing of chains. If this value is
    // -1, there is no next entry, and this entry marks the end of a chain.  
    int64_t collided_entry_index;
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

    // Contains indexes inside of the cellar that been freed by an entry being
    // removed, and can be used.
    Tundra_DynamicStackUInt64 available_cellar_indexs; 

} TUNDRA_HSHTBL_TBLSTRUCT_SIG; // HashTable


// Private Methods -------------------------------------------------------------



// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes the HashTable, allocating initial memory and setting 
 * internal components.
 * 
 * @param table Table to initialize.
 */
void TUNDRA_HSHTBL_FUNC_SIG(_init)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table)
{

}

/**
 * @brief Adds a pair to the table, where `value` is associated to the 
 * hash of `key`.
 * 
 * @param key Key of the pair, used as a lookup for `value`
 * @param value Value of the pair, retrieved when `key` is looked up.
 */
void TUNDRA_HSHTBL_FUNC_SIG(_add)(TUNDRA_HSHTBL_TBLSTRUCT_SIG *table,
    TUNDRA_HSHTBL_KEYTYPE key, TUNDRA_HSHTBL_VALUETYPE value)
{

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