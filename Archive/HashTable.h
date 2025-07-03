/**
 * @file HashTable.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic container for storing key value pairs.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_HASHTABLE_H
#define TUNDRA_HGUARD_HASHTABLE_H

#include <stdint.h>

/**
 * @brief Contains data for a single entry in the HashTable. 
 * 
 * Users do not need to interact with this container, it is used internally by 
 * the Tundra_HashTable.
 */
typedef struct Tundra_HashEntry Tundra_HashEntry;

/**
 * @brief Generic container for storing key value pairs.
 * 
 * Users should NOT modify internal variables, they are read-only.
 */
typedef struct
{
    // Sequence of added entries. Indexes into this array correspond to the 
    // hash of an arbitrary value. 
    Tundra_HashEntry *entries;

    // Maximum number of buckets this table contains. 
    uint64_t capacity;

    // Number of entires that are present in the Table. 
    uint64_t num_entries;

} Tundra_HashTable;



#endif // TUNDRA_HGUARD_HASHTABLE_H