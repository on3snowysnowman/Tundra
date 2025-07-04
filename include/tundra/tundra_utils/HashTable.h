

#ifndef TUNDRA_HGUARD_HASHTABLE_H
#define TUNDRA_HGUARD_HASHTABLE_H

#include <stdint.h>

#include "tundra/tundra_utils/MacroUtils.h"

#endif // TUNDRA_HGUARD_HASHTABLE_H


// Macros ----------------------------------------------------------------------

// Key type the HashTable stores.
#ifndef TUNDRA_HSHTBL_KEYTYPE
#define TUNDRA_KEYTYPE int
#else
#define TUNDRA_KEYTYPE TUNDRA_HSHTBL_KEYTYPE
#endif

// Value type the of the HashTable.
#ifndef TUNDRA_HSHTBL_VALUETYPE
#define TUNDRA_VALUETYPE int
#else 
#define TUNDRA_VALUETYPE TUNDRA_HSHTBL_VALUETYPE
#endif

// Name identifier for the specific type instance of the HashTable
#ifndef TUNDRA_HSHTBL_NAME
#define TUNDRA_NAME Dflt
#else 
#define TUNDRA_NAME TUNDRA_HSHTBL_NAME
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

// #ifndef TUNDRA_DYNAMICSTACKInt
// #define TUNDRA_DYNAMICSTACKInt
// #define TUNDRA_DYNSTK_TYPE int
// #define TUNDRA_DYNSTK_NAME Int
// #include "tundra/tundra_utils/DynamicStack.h"
// #undef TUNDRA_DYNSTK_TYPE
// #undef TUNDRA_DYNSTK_NAME
// #endif


#ifndef TUNDRA_DYNAMICARRAYInt
#define TUNDRA_DYNAMICARRAYInt
#define TUNDRA_DYNARR_TYPE int
#define TUNDRA_DYNARR_NAME Int
#include "tundra/tundra_utils/DynamicArray.h"
#undef TUNDRA_DYNARR_TYPE
#undef TUNDRA_DYNARR_NAME
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
    TUNDRA_VALUETYPE value;

    // The index inside the Cellar of the entry that collided with this one.
    // Storing the index in this way allows us to create "chains" of entries
    // that share the same hash but NOT the same value, where each entry
    // points to the next one allowing quick parsing of chains. If this value is
    // -1, there is no next entry, and this entry marks the end of a chain.  
    int64_t collided_entry_index;

} TUNDRA_ENTRYSTRUCT_SIG; // HashEntry


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

    Tundra_DynamicArrayInt arr;

} TUNDRA_TABLESTRUCT_SIG; // HashTable

#undef TUNDRA_VALUETYPE
#undef TUNDRA_KEYTYPE
#undef TUNDRA_NAME
#undef TUNDRA_TABLESTRUCT_SIG
#undef TUNDRA_ENTRYSTRUCT_SIG
#undef TUNDRA_FUNC_SIG
#undef TUNDRA_INTFUNC_SIG