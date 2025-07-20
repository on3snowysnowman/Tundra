/**
 * @file LinkedList.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting, removing, 
 *        and traversing elements.
 * @version 0.1
 * @date 07-15-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tundra/tundra_utils/MacroUtils.h"

#endif // LINKEDLIST_H

#ifndef TUNDRA_LNKLST_TYPE
#define TUNDRA_LNKLST_TYPE_MANUALLY_SET
#define TUNDRA_LNKLST_TYPE int
#endif

#ifndef TUNDRA_LNKLST_NAME
#define TUNDRA_LNKLST_NAME_MANUALLY_SET
#define TUNDRA_LNKLST_NAME Int
#endif


// Macros ---------------------------------------------------------------------

// Full signature of the LinkedList struct.
#define TUNDRA_LNKLST_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_LinkedList, TUNDRA_LNKLST_NAME)

// Full signature of the LinkedList's Node struct.
#define TUNDRA_LNKLST_NODE_SIG \
    TUNDRA_JOIN_TWO_MACROS(InternalTundra_LnkLstNode, TUNDRA_LNKLST_NAME)

// Functions signature for a LinkedList of a given type.
#define TUNDRA_LNKLST_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_LnkLst, TUNDRA_LNKLST_NAME, func_name)

// Internal function signature for a LinkedList of a given type.
#define TUNDRA_LNKLST_INTFUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(InternalTundra_LnkLst, TUNDRA_LNKLST_NAME, func_name)

// Size in bytes of the specified LinkedList type
#define TUNDRA_LNKLST_TYPE_SIZE sizeof(TUNDRA_LNKLST_TYPE)

// -----------------------------------------------------------------------------

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
 * @brief Contains data for a single node in the LinkedList.
 * 
 * Users do not need to interact with this container, it is used internally by 
 * the Tundra_LinkedList.
 */
typedef struct 
{
    // Value this node stores. 
    TUNDRA_LNKLST_TYPE value;

    // Index of the next node in the chain. No "sentinel" value is needed to 
    // flag that this node does not point to another index, as the size of the
    // nodes array will accurately determine the length of the chain and when 
    // to stop parsing.
    uint64_t next_node_index;

} TUNDRA_LNKLST_NODE_SIG;


// Define implementation for the array of nodes.
#define TUNDRA_DYNARR_TYPE TUNDRA_LNKLST_NODE_SIG
#define TUNDRA_DYNARR_NAME TUNDRA_JOIN_TWO_MACROS(LnkLstNode, \
    TUNDRA_LNKLST_NAME)

// Define the struct signature for the DynamicArray of the LinkedListNode 
// manually, since the struct sig
#define TUNDRA_LNKLST_NODEARR_STRUCT_SIG \
    TUNDRA_JOIN_TWO_MACROS(Tundra_DynamicArray, \
        TUNDRA_JOIN_TWO_MACROS(LnkLstNode, TUNDRA_LNKLST_NAME))

#define TUNDRA_LNKLST_NODEARR_FUNC_SIG(func_name) \
    TUNDRA_DEFINE_FUNC_SIG(Tundra_DynArr, TUNDRA_JOIN_TWO_MACROS(LnkLstNode, \
    TUNDRA_LNKLST_NAME), func_name)

#include "tundra/tundra_utils/DynamicArray.h"
#undef TUNDRA_DYNARR_TYPE
#undef TUNDRA_DYNARR_NAME

/**
 * @brief Dynamically linked container for efficient insertion, removal, 
 * and traversal of elements.
 * 
 * This LinkedList uses an array-based structure where nodes are stored 
 * contiguously in memory, each referencing the index of the next node. 
 * Node order in the array does not necessarily reflect insertion order,
 * traversal follows the chain of indices rather than array order.
 * 
 * This design improves cache locality and reduces the number of heap 
 * allocations by minimizing malloc calls. Nodes are allocated in a block 
 * rather than individually. 
 * 
 * The tradeoff is that resizing the underlying array may require 
 * reallocating and copying memory when capacity is exceeded. However, 
 * I believe this is offset by the performance benefits of fewer allocations and 
 * better memory locality.
 * 
 * Internal variables are read-only.
 */
typedef struct
{
    // Array of nodes making up the linked list.
    // Tundra_DynamicArrayUInt64 nodes;
    Tundra_DynamicArray arr;

    // Indexs inside the `nodes` array that have been removed and their index is
    // free to be reused.
    Tundra_DynamicStackUInt64 freed_indexes;    

    // Index inside the `nodes` array of the head node of the chain.
    uint64_t head_index;
    
} TUNDRA_LNKLST_STRUCT_SIG;


// Private Methods ------------------------------------------------------------

/**
 * @brief Underlying initialization method, initializes the list and allocates
 * `init_capacity` elements for the node array.
 * 
 * @param list List to initialize.
 * @param init_capacity Initial capacity of nodes to allocate for.
 */
static void TUNDRA_LNKLST_INTFUNC_SIG(_underlying_init)(
    TUNDRA_LNKLST_STRUCT_SIG *list, uint64_t init_capacity)
{
    Tundra_DynArrUInt64_init(&list->nodes, init_capacity)
}



// Public Methods -------------------------------------------------------------

static inline void TUNDRA_LNKLST_FUNC_SIG(_init)(
    TUNDRA_LNKLST_STRUCT_SIG *list) {}

static inline void TUNDRA_LNKLST_FUNC_SIG(_init_with_capacity)(
    TUNDRA_LNKLST_STRUCT_SIG *list, uint64_t init_capacity)
