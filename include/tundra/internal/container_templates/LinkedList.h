/**
 * @file LinkedList.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing
 * elements.
 * @version 0.1
 * @date 2025-12-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/NumLimits.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/utils/Math.h"

#ifndef TUNDRA_LNKLST_H
#define TUNDRA_LNKLST_H
#define TUNDRA_LNKLST_DEF_CAP 4
#define TUNDRA_LNKLST_SENTINEL_IDX 0
#endif

#define TUNDRA_MAX_ELEMS_NAME TUNDRA_CONCAT3(TUNDRA_LNKLST, TUNDRA_TYPENAME, \
    _MAX_ELEMS)

// Type and Function Name Macros -----------------------------------------------
#define TUNDRA_LIST_NAME TUNDRA_CONCAT(Tundra_LinkedList, TUNDRA_TYPENAME)
#define TUNDRA_NODE_NAME TUNDRA_CONCAT(InTundra_LinkedListNode, TUNDRA_TYPENAME)
#define TUNDRA_ITER_NAME TUNDRA_CONCAT(Tundra_LinkedListIterator, \
    TUNDRA_TYPENAME)

#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_LnkLst, TUNDRA_TYPENAME, \
    _##name)
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_LnkLst, \
    TUNDRA_TYPENAME, _##name)
#define TUNDRA_ITER_FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_LnkLstIter, \
    TUNDRA_TYPENAME, _##name)

#define TUNDRA_NODE_SIZE sizeof(TUNDRA_EXPAND(TUNDRA_NODE_NAME))


#ifdef __cplusplus
extern "C" {
#endif


// Containers ------------------------------------------------------------------

/**
 * @brief Represents a single Node in the Linked List. Stores a single element 
 * as well as its next and previous Node, if it has them.
 * 
 */
typedef struct TUNDRA_NODE_NAME
{
    // Payload of the Node.
    TUNDRA_TYPE datum;

    // Index of the next Node, or TUNDRA_LNKLST_SENTINEL_IDX if there isn't one.
    uint64 next;

    // Index of the previous Node, or TUNDRA_LNKLST_SENTINEL_IDX if there isn't 
    // one.
    uint64 prev;
} TUNDRA_NODE_NAME;

/**
 * @brief Dynamically linked container for efficiently inserting and removing 
 * elements.
 * 
 * This LinkedList is not implemented as a chain of separately allocated Nodes.
 * It uses a contiguous array to hold all the Nodes, and each Node points 
 * dynamically to its next and previous Nodes using indexes into said array.
 * This means that the List will need to be periodically resized and reallocated
 * as elements are added, however this provides the benefit of cache locality 
 * with consecutive indexing as well as no need for individual malloc calls per 
 * each element addition. This means that there are methods such as `reserve` 
 * that aim to reduce the impact / increase interval of allocations when adding 
 * many elements at one time.  
 */
typedef struct TUNDRA_LIST_NAME
{
    // Array of nodes.
    TUNDRA_NODE_NAME *nodes;

    // Indexes inside the array of Nodes that have been freed and can be reused.
    Tundra_DynamicStacku64 freed_idxs; 

    // Index of the head Node, or TUNDRA_LNKLST_SENTINEL_IDX if there isn't one.
    uint64 head_idx;

    // Index of the tail Node, or TUNDRA_LNKLST_SENTINEL_IDX if there isn't one.
    uint64 tail_idx;

    // Number of Nodes in the List.
    uint64 num_node;

    // Current number of elements that can be added to the List before resizing 
    // is required.
    uint64 cap;

    // Current allocated capacity in bytes. Do note that cap * 
    // sizeof(TUNDRA_TYPE) may be less than cap_bytes if TUNDRA_TYPE size is not 
    // a power of 2. 
    uint64 cap_bytes;

} TUNDRA_LIST_NAME;


// Internal Methods

/**
 * @brief Updates the tail index of the List as well as the Sentinel's previous 
 * Node pointer to point to the new tail index.
 * 
 * @param list List to update.
 * @param idx New tail index.
 */
static inline void TUNDRA_INT_FUNC_NAME(update_tail_idx)(
    TUNDRA_LIST_NAME *list, uint64 idx)
{
    list->tail_idx = idx;
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].prev = idx;
}

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` elems and sets container components.
 * 
 * @param list List to initialize.
 * @param init_cap Initial capacity in elements.
 */
static inline void TUNDRA_INT_FUNC_NAME(init)(
    TUNDRA_LIST_NAME *list, uint64 init_cap)
{
    Tundra_DynStku64_init(&list->freed_idxs);

    const uint64 INIT_CAP_BYTE = Tundra_ceil_pow2(init_cap * TUNDRA_NODE_SIZE);

    list->nodes = (TUNDRA_NODE_NAME*)Tundra_alloc_mem(INIT_CAP_BYTE);

    // Initialize Sentinel. Points to itself, has no next.
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;
    // `prev` member of the Sentinel is set with the `update_tail_idx` call.

    list->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    TUNDRA_INT_FUNC_NAME(update_tail_idx)(list, TUNDRA_LNKLST_SENTINEL_IDX);

    list->num_node = 0;
    list->cap = INIT_CAP_BYTE / sizeof(TUNDRA_NODE_SIZE);
    list->cap_bytes = INIT_CAP_BYTE;
}


// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes a List with default capacity. Allocates memory and sets 
 * internal components.
 * 
 * Only initialize a List once. If an already initialized List is called with 
 * init, undefined behavior may occur.
 * 
 * @param list List to init.
 */
static inline void TUNDRA_FUNC_NAME(init)(TUNDRA_LIST_NAME *list)
{
    TUNDRA_INT_FUNC_NAME(init)(list, TUNDRA_LNKLST_DEF_CAP);
}

/**
 * @brief Initializes a List with set capacity. Allocates memory and sets
 * internal components.
 * 
 * If `init_cap` is 0, the List is initialized with default capacity.
 * 
 * Only initialize a List once. If an already initialized List is called with
 * init, undefined behavior may occur.
 * 
 * @param list List to init.
 * @param init_cap Specified initial capacity.
 */
static inline void TUNDRA_FUNC_NAME(init_w_cap)(TUNDRA_LIST_NAME *list, 
    uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_LNKLST_DEF_CAP : init_cap;

    TUNDRA_INT_FUNC_NAME(init)(list, init_cap);
}

static inline void TUNDRA_FUNC_NAME(init_w_elems)(TUNDRA_LIST_NAME *list, 
    const TUNDRA_TYPE *elements, uint64 num_elem)
{
    Tundra_DynStkut64_init(&list->freed_idxs);

    // Number of elements needed for allocating with an extra space in mind for
    // the sentinel.
    const uint64 NUM_ELEM_W_SENT = num_elem + 1;
    
    const uint64 INIT_CAP_BYTE = 
        Tundra_ceil_pow2(NUM_ELEM_W_SENT * TUNDRA_NODE_SIZE);

    list->nodes = (TUNDRA_NODE_NAME*)Tundra_alloc_mem(INIT_CAP_BYTE);
    list->cap = INIT_CAP_BYTE / sizeof(TUNDRA_NODE_NAME);
    list->cap_bytes = INIT_CAP_BYTE;

    const uint64 FIRST_NODE_IDX = 1; // Index 0 is reserved for Sentinel.

    // -- Initialize Sentinel -- 

    
}
