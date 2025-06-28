/**
 * @file HashTable.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic container for storing key value pairs.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */


 /**
  * Temp comment for remembering how I'm going to design this when I get to it:
  * 
  * HashTable will handle collisions by placing entries in a "Cellar" that is 
  * a reserved space ~20% of the end of the Table. When a collision entry is 
  * placed in the Cellar, update an index tracking variable that this was the 
  * last index a collision was placed at, so the next collision can know where
  * it needs to look to place the next collision in the Cellar, rather than 
  * needing to iterate over the entire cellar from the bottom until a free pos
  * is found. Additionally, use a queue to track indexes that have been freed
  * in the Cellar that can be used for the next collision. If the queue is empty
  * when a collision happens, the collision will be placed right after the 
  * greatest position that a collision has been placed at, since it will be 
  * guaranteed that the remaining space after that greatest position to the end
  * of the cellar will be free.
  * 
  */


#include "tundra/tundra_tools/HashTable.h"

#include <stdbool.h>

// Limit that the (number of elements added / capacity) ratio of a HashTable can
// reach before being resized and rehashed. 
#define LOAD_FACTOR_THRESHOLD 0.7


struct Tundra_HashEntry
{
    // Value of this hash entry. If this pointer is set to NULL, this entry is
    // not currently used by the Table, and contains no valid data.
    void* value; 
    
    // A pointer to an entry inside the "Cellar" of the table which had collided 
    // with this entry and used linear probing to find the next position. If 
    // this value is NULL, there is no next value, flagging the end of the 
    // HashEntry "chain" that all share the same Hash. 
    Tundra_HashEntry* next_entry_with_shared_hash;
};


