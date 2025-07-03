

#ifndef TUNDRA_HGUARD_HASHTABLE_H
#define TUNDRA_HGUARD_HASHTABLE_H

#include <stdint.h>

#include "tundra/tundra_utils/MacroUtils.h"

#endif // TUNDRA_HGUARD_HASHTABLE_H


// Macros ----------------------------------------------------------------------

// Data type the HashTable stores.
#ifndef TUNDRA_TYPE
#define TUNDRA_TYPE_MANUALLY_SET
#define TUNDRA_TYPE int
#endif

// Name identifier for the specific type instance of the HashTable
#ifndef TUNDRA_NAME
#define TUNDRA_NAME_MANUALLY_SET
#define TUNDRA_NAME Dflt
#endif

// Full signature of the HashTable struct.
#define TUNDRA_TABLESTRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_HashTable, TUNDRA_NAME)


// Full signature of the HashEntry struct.
#define TUNDRA_ENTRYSTRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_HashEntry, TUNDRA_NAME)

// Function signature for a HashTable of a given type.
#define TUNDRA_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_HshTbl, TUNDRA_NAME, func_name)

// Internal function signature for a HashTable of a given type
#define TUNDRA_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(InternalTundra_HshTbl, TUNDRA_NAME , func_name)


// Tundra Library Container Definitions ----------------------------------------


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
    TUNDRA_TYPE value;

    // The index inside the Cellar of the entry that collided with this one.
    // Storing the index in this way allows us to create "chains" of entries
    // that share the same hash but NOT the same value, where each entry
    // points to the next one allowing quick parsing of chains. If this value is
    // -1, there is no next entry, and this entry marks the end of a chain.  
    int64_t collided_entry_index;

} TUNDRA_ENTRYSTRUCT_SIG;


/**
 * @brief Expanding container for storing key-value pairs.
 * 
 *  * Internal variables are read-only.
 */
typedef struct
{
    TUNDRA_ENTRYSTRUCT_SIG* data;

    // Number of entries added to the "top" of the HashTable.
    uint64_t num_entries_top; 

    // Capacity in HashEntrys of the "top" of the HashTable 
    uint64_t top_capacity;

    // Number of entires added to the "cellar" of the HashTable.
    uint64_t num_entries_cellar;

    // Capacity in HashEntrys of the "cellar" of the HashTable.
    uint64_t cellar_capacity;



} TUNDRA_TABLESTRUCT_SIG;