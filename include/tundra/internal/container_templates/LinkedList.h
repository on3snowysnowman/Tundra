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
#endif


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

// Macro for defining a type specific variable for the maximum number of 
// elements allowed in the List for hardware size constraints.
#define TUNDRA_MAX_ELEMS_NAME TUNDRA_CONCAT3(TUNDRA_LNKLST, TUNDRA_TYPENAME, \
    _MAX_ELEMS)

// Expression to get the head index variable
#define TUNDRA_HEAD_IDX_EXPR list->nodes[TUNDRA_SENTINEL_IDX].next
// Expression to get the tail index variable
#define TUNDRA_TAIL_IDX_EXPR list->nodes[TUNDRA_SENTINEL_IDX].prev

#define TUNDRA_SENTINEL_IDX 0


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
    // Index of the next Node, or TUNDRA_SENTINEL_IDX if there isn't one.
    uint64 next;

    // Index of the previous Node, or TUNDRA_SENTINEL_IDX if there isn't 
    // one.
    uint64 prev;

    // Payload of the Node.
    TUNDRA_TYPE datum;
} TUNDRA_NODE_NAME;

// Max number of elements 
static const uint64 TUNDRA_MAX_ELEMS_NAME = 
    TUNDRA_UINT64_MAX / TUNDRA_NODE_SIZE;

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
    // Indexes inside the array of Nodes that have been freed and can be reused.
    Tundra_DynamicStacku64 freed_idxs; 
    
    // Array of nodes.
    TUNDRA_NODE_NAME *nodes;

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

/**
 * @brief Iterator for the LinkedList.
 * 
 */
typedef struct TUNDRA_ITER_NAME
{
    // Pointer to the List being iterated over.
    TUNDRA_LIST_NAME *list;

    // Current index into the List's Node array.
    uint64 index;
} TUNDRA_ITER_NAME;


// Internal Methods ------------------------------------------------------------

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

    // Initialize Sentinel. Points to itself since the List is empty.
    TUNDRA_HEAD_IDX_EXPR = TUNDRA_SENTINEL_IDX;
    TUNDRA_TAIL_IDX_EXPR = TUNDRA_SENTINEL_IDX;

    // `prev` member of the Sentinel is set with the `update_tail_idx` call.

    // list->head_idx = TUNDRA_SENTINEL_IDX;
    // TUNDRA_INT_FUNC_NAME(update_tail_idx)(list, TUNDRA_SENTINEL_IDX);

    list->num_node = 0;
    list->cap = INIT_CAP_BYTE / TUNDRA_NODE_SIZE;
    list->cap_bytes = INIT_CAP_BYTE;
}

/**
 * @brief Copies all Nodes from `src` to `dst`. 
 * 
 * Assumes both Lists are initialized and `dst` has enough capacity to hold 
 * `src`'s Nodes. Does not touch any other container components but the Nodes 
 * array.
 * 
 * @param src List to source Nodes from.
 * @param dst List to copy Nodes to.
 */
static inline void TUNDRA_INT_FUNC_NAME(copy_nodes)(const TUNDRA_LIST_NAME *src,
    TUNDRA_LIST_NAME *dst)
{
    // Copy Sentinel
    dst->nodes[TUNDRA_SENTINEL_IDX] = 
        src->nodes[TUNDRA_SENTINEL_IDX];

    // Copy the data Nodes, start at the first Node
    const TUNDRA_NODE_NAME *src_parsed_node = 
        &src->nodes[src->nodes[TUNDRA_SENTINEL_IDX].next];
    TUNDRA_NODE_NAME *dst_parsed_node = 
        &dst->nodes[dst->nodes[TUNDRA_SENTINEL_IDX].next];

    while(src_parsed_node != src->nodes + TUNDRA_SENTINEL_IDX)
    {

    #if TUNDRA_NEEDS_CUSTOM_COPY

        TUNDRA_COPY_CALL_SIG(&src_parsed_node->datum, &dst_parsed_node->datum);
    #else
        
        dst_parsed_node->datum = src_parsed_node->datum;
    #endif

        dst_parsed_node->next = src_parsed_node->next;
        dst_parsed_node->prev = src_parsed_node->prev;

        src_parsed_node = src->nodes + src_parsed_node->next;
        dst_parsed_node = dst->nodes + dst_parsed_node->next;
    }
}

#if TUNDRA_NEEDS_CUSTOM_FREE
/**
 * @brief Frees all Nodes in a List by iterating through each one calling the 
 * custom free method on its datum.
 * 
 * @param list List to free Nodes of.
 */
static inline void TUNDRA_INT_FUNC_NAME(custom_free_nodes)
    (TUNDRA_LIST_NAME *list)
{
    TUNDRA_NODE_NAME *parsed_node = list->nodes + list->head_idx;

    // Iterate through the list, custom freeing each element
    while(parsed_node != list->nodes + TUNDRA_SENTINEL_IDX)
    {
        TUNDRA_FREE_CALL_SIG(&parsed_nodes->datum);
        parsed_node = list->nodes + parsed_node->next;
    }
}
#endif

/**
 * @brief Allocates new memory for the List and moves existing Nodes to the new
 * memory. Updates the internal components of the List.
 * 
 * Assumes the number of Nodes in the List is greater than 0, also assumes 
 * `new_cap_bytes` is a power of 2.
 * 
 * @param list List to reallocate.
 * @param new_cap_bytes New capacity in bytes.
 */
static inline void TUNDRA_INT_FUNC_NAME(alloc_move_mem)(TUNDRA_LIST_NAME *list, 
    uint64 new_cap_bytes)
{
    // Clear the freed indexes since we are moving all the existing Nodes into
    // one consecutive line in the new memory. 
    Tundra_DynStku64_clear(&list->freed_idxs);

    TUNDRA_NODE_NAME *new_mem = 
        (TUNDRA_NODE_NAME*)Tundra_alloc_mem(new_cap_bytes);

    // Current parsed Node into the old memory of the List. Starts at the first
    // Node which is always the head Node.
    TUNDRA_NODE_NAME *src_parsed_node = 
        &list->nodes[TUNDRA_HEAD_IDX_EXPR]; 

    // Index where the Head Node will be placed inside the new memory, 1 after
    // the Sentinel.
    enum { HEAD_NODE_IDX = TUNDRA_SENTINEL_IDX + 1 }; 

    // Current index into the new memory array. We start at 1 past the 
    // Sentinel index, since the Sentinel is reserved at the 0th index.
    uint64 i = HEAD_NODE_IDX;

    // While the parsed Node inside the old memory does not reach the Sentinel.
    while(src_parsed_node != &list->nodes[TUNDRA_SENTINEL_IDX])
    {
        // Pointer to the Node inside the new memory. This is the Node that we
        // will be copying to this iteration. 
        TUNDRA_NODE_NAME *dst_parsed_node = &new_mem[i];

        #if TUNDRA_NEEDS_CUSTOM_MOVE

        TUNDRA_MOVE_CALL_SIG(&src_parsed_node->datum, &dst_parsed_node->datum);
        #else 

        dst_parsed_node->datum = src_parsed_node->datum;
        #endif

        dst_parsed_node->next = i + 1;
        dst_parsed_node->prev = i - 1;

        // Iterate to the next Node in the old memory.
        src_parsed_node = &list->nodes[src_parsed_node->next];
    
        // Iterate to the next position in the new memory. Since this is new
        // memory and we are placing the new Nodes in a line, we can just 
        // increment the index linearly.
        ++i;
    }

    // Index of the tail Node in the Nodes array. We use `num_elem` instead of
    // typical `num_elem` - 1 indexing here since we add 1 to account for the 
    // Sentinel existing at the beginning, shifting indexes right 1.
    const uint64 TAIL_NODE_IDX = list->num_node;

    // Set the tail Node's next value to the Sentinel, since it has no next.
    list->nodes[TAIL_NODE_IDX].next = TUNDRA_SENTINEL_IDX;

    // Free the old memory. Since the custom move call would've handled the old 
    // Node datums if they required custom handling, we don't need to worry 
    // about iterating through each Node and custom freeing its datum.
    Tundra_free_mem((void*)list->nodes);

    list->nodes = new_mem;
    list->cap_bytes = new_cap_bytes;
    list->cap = list->cap_bytes / TUNDRA_NODE_SIZE;

    // Update the Sentinel.
    new_mem[TUNDRA_SENTINEL_IDX].next = HEAD_NODE_IDX;
    TUNDRA_TAIL_IDX_EXPR = TAIL_NODE_IDX;
}

/**
 * @brief Checks if a List has filled its allocated capacity, expanding and 
 * reallocating if it has
 * 
 * @param list List to handle.
 */
static inline void TUNDRA_INT_FUNC_NAME(check_handle_exp)(
    TUNDRA_LIST_NAME *list)
{
    // Add 1 to account for the Sentinel.
    if(list->num_node + 1 < list->cap) { return; }

    // Prevent overflow
    if(list->cap_bytes > (TUNDRA_UINT64_MAX / 2))
    {
        TUNDRA_FATAL("Capacity overflow on expansion."); 
    }

    // Double the capacity. Since the capacity in bytes is always a power of 2,
    // doubling it will maintain this property.
    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(list, 2 * list->cap_bytes);
}

/**
 * @brief Finds the first available index that a Node can be placed at.
 * 
 * Assumes there is at least one spot available for a new Node in the Node 
 * array.
 * 
 */
static inline uint64 TUNDRA_INT_FUNC_NAME(get_avail_index)(
    TUNDRA_LIST_NAME *list)
{
    // If there are no available indexes that have been freed.
    if(Tundra_DynStku64_is_empty(&list->freed_idxs))
    {
        // Since there are no freed positions inside the array of Nodes, the 
        // first available position will always be at the end of the filled 
        // array of Nodes. We add 1 here to account for the Sentinel taking 
        // up the zeroth index.
        return list->num_node + 1;
    }

    // -- Fetch an available index from the stack of freed indexes. --

    const uint64 avail_index = *Tundra_DynStku64_front_cst(&list->freed_idxs);
    Tundra_DynStku64_pop(&list->freed_idxs);
    return avail_index;
}
/**
 * @brief Finds a Node at a position in the List by iterating from the tail Node
 * and returns its index into the Nodes array.
 * 
 * Assumes the position is valid.
 * 
 * @param list List to get index from.
 * @param pos Position in the List.
 */
static inline uint64 TUNDRA_INT_FUNC_NAME(find_idx_from_tail)(
    const TUNDRA_LIST_NAME *list, uint64 pos)
{
    // Start parsing at the Sentinel Node.
    TUNDRA_NODE_NAME *parsed_node = &list->nodes[TUNDRA_SENTINEL_IDX];

    // Iterate to the Node positionally after the Node to find.
    for(uint64 i = 0; i < list->num_node - pos - 1; ++i)
    {
        parsed_node = &list->nodes[parsed_node->prev];
    }

    // parsed_node is one after the Node to find, so use it's prev member to 
    // get the index of the searched for Node.
    return parsed_node->prev;
}

/**
 * @brief Finds a Node at a position in the List by iterating from the head Node
 * and returns its index into the Nodes array.
 * 
 * Assumes the position is valid.
 * 
 * @param list List to get index from.
 * @param pos Position in the List.
 */
static inline uint64 TUNDRA_INT_FUNC_NAME(find_idx_from_head)(
    const TUNDRA_LIST_NAME *list, uint64 pos)
{
    // Start parsing at the Sentinel Node.
    TUNDRA_NODE_NAME *parsed_node = &list->nodes[TUNDRA_SENTINEL_IDX];

    // Iterate to the Node right before the Node to find. 
    for(uint64 i = 0; i < pos; ++i)
    {
        parsed_node = &list->nodes[parsed_node->next];
    }

    // parsed_node is one before the Node to find, so use it's next member to 
    // get the index of the searched for Node.
    return parsed_node->next;
}

/**
 * @brief Finds a Node at a position in the List and returns its index into 
 * the Nodes array. 
 * 
 * Assumes the position is valid.
 * 
 * @param list List to get Node from.
 * @param pos Position in the List.
 */
static inline uint64 TUNDRA_INT_FUNC_NAME(find_idx_of_node)(
    const TUNDRA_LIST_NAME *list, uint64 pos)
{
    return (pos < list->num_node / 2) ? 
        TUNDRA_INT_FUNC_NAME(find_idx_from_head)(list, pos) : 
        TUNDRA_INT_FUNC_NAME(find_idx_from_tail)(list, pos);
}

/**
 * @brief Finds a Node at a position in the List. Determines whether to start 
 * searching from the beginning or end depending on the position.
 * 
 * @param list List to get Node from.
 * @param pos Position in the List.
 */
static inline TUNDRA_NODE_NAME* TUNDRA_INT_FUNC_NAME(get_node_at_pos)(
    TUNDRA_LIST_NAME *list, uint64 pos)
{
    // return (pos < list->num_node / 2) ?
    //     TUNDRA_INT_FUNC_NAME(find_from_start)(list, pos) : 
    //     TUNDRA_INT_FUNC_NAME(find_from_end)(list, pos);
    return &list->nodes[TUNDRA_INT_FUNC_NAME(find_idx_of_node)(list, pos)];
}

/**
 * @brief Finds a Node at a position in the List. Determines whether to start 
 * searching from the beginning or end depending on the position. 
 * 
 * Returned Node pointer is const.
 * 
 * @param list List to get Node from.
 * @param pos Position in the List.
 */
static inline const TUNDRA_NODE_NAME* TUNDRA_INT_FUNC_NAME(get_node_at_pos_cst)(
    const TUNDRA_LIST_NAME *list, uint64 pos)
{
    // return (pos < list->num_node / 2) ?
    //     TUNDRA_INT_FUNC_NAME(find_from_start_cst)(list, pos) : 
    //     TUNDRA_INT_FUNC_NAME(find_from_end_cst)(list, pos);
    return &list->nodes[TUNDRA_INT_FUNC_NAME(find_idx_of_node)(list, pos)];
}

/**
 * @brief Expands the List to ensure it has the capacity to store 
 * `num_extra_elem`additional elements.
 * 
 * Assumes that the current number of elements plus `num_extra_elems` exceeds 
 * the current capacity.
 * 
 * @param list List to expand.
 * @param num_extra_elem Number of extra elements.
 */
static inline void TUNDRA_INT_FUNC_NAME(reserve_for)(TUNDRA_LIST_NAME *list, 
    uint64 num_extra_elem)
{
    // -1 to account for the Sentinel
    if (num_extra_elem > TUNDRA_MAX_ELEMS_NAME - list->num_node - 1) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    }

    // +1 for the Sentinel.
    const uint64 TOTAL_ELEM = list->num_node + num_extra_elem + 1;

    if(TOTAL_ELEM > (TUNDRA_UINT64_MAX / TUNDRA_NODE_SIZE)) 
    {
        TUNDRA_FATAL("Capacity overflow on reserve.");
        return;
    
    }

    const uint64 TOT_REQ_BYTE = TOTAL_ELEM * TUNDRA_NODE_SIZE;

    // Calculate new capacity as the next power of 2 that can hold the required
    // bytes.
    const uint64 NEW_CAP_BYTE = Tundra_ceil_pow2(TOT_REQ_BYTE);

    TUNDRA_INT_FUNC_NAME(alloc_move_mem)(list, NEW_CAP_BYTE);
}

/**
 * @brief Internal resize method called by the public resize method for when 
 * a List is being resized to a size smaller than its current number of 
 * contained elements.
 * 
 * Assumes `num_elem` does not account for the Sentinel.
 * 
 * @param list List to resize.
 * @param num_elem Number of element to resize for, less than the List's current
 * elements.
 */
static inline void TUNDRA_INT_FUNC_NAME(resize_smaller)(TUNDRA_LIST_NAME *list, 
    uint64 num_elem)
{
    // Get the index of the last Node in the List that is being kept, since
    // we are shrinking the List. 
    const uint64 IDX_OF_LAST_NODE = 
        TUNDRA_INT_FUNC_NAME(find_idx_of_node)(list, num_elem - 1);
        
    TUNDRA_NODE_NAME *last_kept_node = &list->nodes[IDX_OF_LAST_NODE];
    
    #if TUNDRA_NEEDS_CUSTOM_FREE

    TUNDRA_NODE_NAME *parsed_node = last_keep_node;

    // Iterate through each Node freeing its datum.
    for(uint64 i = num_elem - 1; i < list->num_node; ++i)
    {
        TUNDRA_FREE_CALL_SIG(&parsed_node->datum);

        parsed_node = &list->nodes[parsed_node->next];
    }

    #endif

    // Set the next member of the last kept Node to the Sentinel index, 
    // culling the rest of the list.
    last_kept_node->next = TUNDRA_SENTINEL_IDX;

    // Update the Sentinel to point to the new last Node.
    TUNDRA_TAIL_IDX_EXPR = IDX_OF_LAST_NODE;
}

/**
 * @brief Internal resize method called by the public resize method for when 
 * a List is being resized to a size larger than its current number of 
 * contained elements.
 * 
 * Assumes `num_elem` does not account for the Sentinel.
 * 
 * @param list List to resize.
 * @param num_elem Number of element to resize for, less than the List's current
 * elements.
 */
static inline void TUNDRA_INT_FUNC_NAME(resize_larger)(TUNDRA_LIST_NAME *list,
    uint64 num_elem)
{
    // If we don't have room for the elements, we need to reallocate.
    // +1 to account for Sentinel.
    if(num_elem + 1 > list->cap)
    {   
        // +1 for Sentinel
        const uint64 NEW_CAP_BYTES =    
            Tundra_ceil_pow2((num_elem + 1) * TUNDRA_NODE_SIZE);

        TUNDRA_INT_FUNC_NAME(alloc_move_mem)(list, NEW_CAP_BYTES);
    }

    // -- Starting at the tail Node, add new Nodes (since we are resizing to
    // a larger List size), updating their next and prev pointers, extending the
    // List chain.

    // Keep track of the previous Node index so we can update each newly added
    // Node's `prev` member. Start at the tail index, so that the first Node to 
    // be added will point back to the previous tail.
    uint64 prev_node_idx = TUNDRA_TAIL_IDX_EXPR;

    const uint64 NUM_NODE_TO_ADD = num_elem - list->num_node;

    for(uint64 i = 0; i < NUM_NODE_TO_ADD; ++i)
    {
        const uint64 AVAIL_IDX = TUNDRA_INT_FUNC_NAME(get_avail_index)(list);
     
        // Update the new Node to point back to the previous Node.
        list->nodes[AVAIL_IDX].prev = prev_node_idx;

        // Update the previous Node to point forward to the new Node.
        list->nodes[prev_node_idx].next = AVAIL_IDX;

        // Intentionally leave the `next` member of the new Node as junk. Either
        // it will be updated on the next for loop iteration when we construct 
        // a new Node, or if this is the last one, it will be updated right 
        // after the loop. Saves from having to write the `next` member as 
        // the Sentinel index, then immediately writing it to the next 
        // constructed Node (if there is one).

        // Update hte previous index to the new Node, as we will be jumping 
        // to creating a new one next for loop iteration.
        prev_node_idx = AVAIL_IDX;

        // Need to update number of Nodes inside the loop, since the 
        // `get_avail_index` call at the top requires this value to be accurate.
        ++list->num_node;
    }

    // Update the last added Node to point next to the Sentinel.
    list->nodes[prev_node_idx].next = TUNDRA_SENTINEL_IDX;

    // Update the Sentinel to point back to this new tail Node.
    TUNDRA_TAIL_IDX_EXPR = prev_node_idx;
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

/**
 * @brief Initializes a List with initial elements. Allocates memory and sets 
 * internal components.
 * 
 * Only initialize a List once. If an already initialized List is called with 
 * init, undefined behavior may occur.
 * 
 * The number of initial elements must be greater than 0. 
 * 
 * @param list List to initialize.
 * @param elems Array of elements to copy in.
 * @param num_elem Number of elements in `elems`. Must be greater than 0.
 */
static inline void TUNDRA_FUNC_NAME(init_w_elems)(TUNDRA_LIST_NAME *list, 
    const TUNDRA_TYPE *elems, uint64 num_elem)
{
    Tundra_DynStku64_init(&list->freed_idxs);

    // Number of elements needed for allocating with the Sentinel.
    const uint64 NUM_ELEM_W_SENT = num_elem + 1;

    const uint64 INIT_CAP_BYTE = 
        Tundra_ceil_pow2(NUM_ELEM_W_SENT * TUNDRA_NODE_SIZE);

    list->nodes = (TUNDRA_NODE_NAME*)Tundra_alloc_mem(INIT_CAP_BYTE);
    list->cap = INIT_CAP_BYTE / sizeof(TUNDRA_NODE_NAME);
    list->cap_bytes = INIT_CAP_BYTE;

    // Index where the head Node will be  placed, which is 1 after the 
    // Sentinel.
    enum { HEAD_NODE_IDX = TUNDRA_SENTINEL_IDX + 1 };

    // Start at 1 to skip the Sentinel index. Since we are incrementing our 
    // node index array by 1 to start 1 ahead of the Sentinel, when we index 
    // into the initial elements array, we need to subtract by 1 to account for 
    // this, so we are grabbing the zeroth index, the first element of the 
    // initial elements array when we are placing it in the first Node position
    // after the Sentinel, which is the 1th place, not 0th. 
    for(uint64 i = 1; i < num_elem + 1; ++i)
    {
    #if TUNDRA_NEEDS_CUSTOM_COPY

        TUNDRA_COPY_CALL_SIG(&elems[i - 1], &list->nodes[i]);
    #else

        list->nodes[i].datum = elems[i - 1];
    #endif

        list->nodes[i].next = i + 1;
        list->nodes[i].prev = i - 1; 
    }

    // Index of the tail Node in the Nodes array. We use `num_elem` instead of
    // typical `num_elem` - 1 indexing here since we add 1 to account for the 
    // Sentinel existing at the beginning, shifting indexes right 1.
    const uint64 TAIL_NODE_IDX = num_elem;

    // Set the tail Node's next value to the Sentinel, since it has no next.
    list->nodes[TAIL_NODE_IDX].next = TUNDRA_SENTINEL_IDX;
    
    list->num_node = num_elem;

    // Update the Sentinel.
    TUNDRA_HEAD_IDX_EXPR = HEAD_NODE_IDX;
    TUNDRA_TAIL_IDX_EXPR = TAIL_NODE_IDX;
}

/**
 * @brief Initializes a List by deep copying another List. Allocates memory and 
 * sets internal components.
 * 
 * `src` must be an initialized List, and `dst` must be uninitialized.
 * 
 * @param src List to source from, must be initialized.
 * @param dst List to deep copy to, must be uninitialized.
 */
static inline void TUNDRA_FUNC_NAME(init_w_copy)(const TUNDRA_LIST_NAME *src,
    TUNDRA_LIST_NAME *dst) 
{
    // Shallow copy initially, we will deep copy required members next. 
    *dst = *src;

    Tundra_DynStku64_init_w_copy(&src->freed_idxs, &dst->freed_idxs);

    dst->nodes = (TUNDRA_NODE_NAME*)Tundra_alloc_mem(src->cap_bytes);

    TUNDRA_INT_FUNC_NAME(copy_nodes)(src, dst);
}

/**
 * @brief Initializes a List by transferring ownership of resources from another
 * List. `src` is left in an uninitialized state.
 * 
 * `src` must be an initialized List, and `dst` must be uninitialized.
 * 
 * @param src List to source from, must be initialized.
 * @param dst List to transfer resources to, must be uninitialized.
 */
static inline void TUNDRA_FUNC_NAME(init_w_move)(TUNDRA_LIST_NAME *src,
    TUNDRA_LIST_NAME *dst) 
{
    *dst = *src;

    src->nodes = NULL;
    src->num_node = 0;
    src->cap = 0;
    src->cap_bytes = 0;
}

static inline void TUNDRA_FUNC_NAME(free)(TUNDRA_LIST_NAME *list)
{
    Tundra_DynStku64_free(&list->freed_idxs);
    
#if TUNDRA_NEEDS_CUSTOM_FREE

    TUNDRA_INT_FUNC_NAME(custom_free_nodes)(list);
#else

    // Since the elements themselves do not require custom free handling, simply
    // free the Node array.
    Tundra_free_mem((void*)list->nodes);
#endif

    list->nodes = NULL;
    list->num_node = 0;
    list->cap = 0;
    list->cap_bytes = 0;
}

/**
 * @brief Deep copies `src` to `dst`. Both Lists must be initialized.
 * dst
 * If the Lists are of the same address, nothing is done.
 * 
 * If the Lists' capacities do not match, `dst` is reallocated to match `src`.
 * 
 * @param src List to source from.
 * @param dst List to deep copy to.
 */
static inline void TUNDRA_FUNC_NAME(copy)(const TUNDRA_LIST_NAME *src, 
    TUNDRA_LIST_NAME *dst)
{
    if(dst == src) { return; }

    Tundra_DynStku64_copy(&src->freed_idxs, &dst->freed_idxs);

    // If capacity bytes do not match, reallocate dst to match src.
    if(dst->cap_bytes != src->cap_bytes)
    {
        TUNDRA_FUNC_NAME(free)(dst);
        // Can simply use the init_w_copy method since dst is now uninitialized.
        TUNDRA_FUNC_NAME(init_w_copy)(src, dst);
        return;
    }

    TUNDRA_INT_FUNC_NAME(copy_nodes)(src, dst);
    dst->num_node = src->num_node;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in 
 * an uninitialized state. Both Lists must be initialized.
 * 
 * If the Lists are of the same address, nothing is done.
 * 
 * @param src List to source from.
 * @param dst List to transfer resources to.
 */
static inline void TUNDRA_FUNC_NAME(move)(TUNDRA_LIST_NAME *src, 
    TUNDRA_LIST_NAME *dst)
{
    if(dst == src) { return; }

    TUNDRA_FUNC_NAME(free)(dst);

    *dst = *src;

    src->nodes = NULL;
    src->num_node = 0;
    src->cap = 0;
    src->cap_bytes = 0;
}

/**
 * @brief Clears the List of all elements. The capacity remains unchanged.
 * 
 * @param list List to clear.
 */
static inline void TUNDRA_FUNC_NAME(clear)(TUNDRA_LIST_NAME *list)
{
    Tundra_DynStku64_clear(&list->freed_idxs);

#if TUNDRA_NEEDS_CUSTOM_FREE
    
    TUNDRA_INT_FUNC_NAME(custom_free_nodes)(list);
#endif

    list->num_node = 0;

    TUNDRA_HEAD_IDX_EXPR = TUNDRA_SENTINEL_IDX;
    TUNDRA_TAIL_IDX_EXPR = TUNDRA_SENTINEL_IDX;
}

/**
 * @brief Adds a copy of an element to the front of the List.
 * 
 * @param list List to add to.
 * @param elem Pointer to the element to add.
 */
static inline void TUNDRA_FUNC_NAME(add_front)(TUNDRA_LIST_NAME *list, 
    const TUNDRA_TYPE *elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(list);

    const uint64 ADD_IDX = TUNDRA_INT_FUNC_NAME(get_avail_index)(list);
    TUNDRA_NODE_NAME *targ_node = &list->nodes[ADD_IDX];

    // -- Copy the element into the datum of the new Node --
    #if TUNDRA_NEEDS_CUSTOM_COPY

        TUNDRA_COPY_CALL_SIG(elem, &targ_node->datum);
    #else

        targ_node->datum = *elem;
    #endif

    // Save the index of the head Node (Node pointed next to by Sentinel)
    const uint64 PREV_HEAD_NODE_IDX = 
        TUNDRA_HEAD_IDX_EXPR;

    // Set the Sentinel to point next to the new Node, the new head Node.
    TUNDRA_HEAD_IDX_EXPR = ADD_IDX;

    // Set the new Node to point back to the Sentinel.
    targ_node->prev = TUNDRA_SENTINEL_IDX;

    // Set the new Node to point next to the previous head Node.
    targ_node->next = PREV_HEAD_NODE_IDX;

    // Update the previous head Node to point back to the new head Node.
    list->nodes[PREV_HEAD_NODE_IDX].prev = ADD_IDX;

    ++list->num_node;
}

/**
 * @brief Adds a copy of an element to the back of the List.
 * 
 * @param list List to add to.
 * @param elem Pointer to the element to add.
 */
static inline void TUNDRA_FUNC_NAME(add_back)(TUNDRA_LIST_NAME *list,
    const TUNDRA_TYPE *elem)
{
    TUNDRA_INT_FUNC_NAME(check_handle_exp)(list);

    const uint64 ADD_IDX = TUNDRA_INT_FUNC_NAME(get_avail_index)(list);
    TUNDRA_NODE_NAME *targ_node = &list->nodes[ADD_IDX];

    // -- Copy the element into the datum of the new Node --
    #if TUNDRA_NEEDS_CUSTOM_COPY

        TUNDRA_COPY_CALL_SIG(elem, &targ_node->datum);
    #else

        targ_node->datum = *elem;
    #endif

    // Save the index of the tail Node (Node pointed prev to by Sentinel)
    const uint64 PREV_TAIL_NODE_IDX = 
        TUNDRA_TAIL_IDX_EXPR;

    // Set the Sentinel to point prev to the new Node, the new tail Node.
    TUNDRA_TAIL_IDX_EXPR = ADD_IDX;

    // Set the new Node to point next to the Sentinel.
    targ_node->next = TUNDRA_SENTINEL_IDX;

    // Set the new Node to point back to the previous tail Node.
    targ_node->prev = PREV_TAIL_NODE_IDX;

    // Update the previous head Node to point next to the new tail Node.
    list->nodes[PREV_TAIL_NODE_IDX].next = ADD_IDX;

    ++list->num_node;
}

/**
 * @brief Inserts a copy of an element at a position
 * 
 * A fatal is thrown if the index is out of range with the List unmodified.
 * 
 * Must search linearly through the list, from either the start or the end to 
 * get to the element position to insert.
 * 
 * @param list List to insert into.
 * @param elem Pointer to the element to copy.
 * @param index Insert index.
 */
static inline void TUNDRA_FUNC_NAME(insert)(TUNDRA_LIST_NAME *list, 
    const TUNDRA_TYPE *elem, uint64 index)
{
    if(index > list->num_node)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for Array of size \"%llu\".", 
            index, list->num_node);
        return;
    }

    TUNDRA_INT_FUNC_NAME(check_handle_exp)(list);

    TUNDRA_NODE_NAME *node_at_insert_idx = 
        TUNDRA_INT_FUNC_NAME(get_node_at_pos)(list, index);

    const uint64 IDX_OF_NODE_AT_INSERT_POS = 
        list->nodes[node_at_insert_idx->prev].next;

    // Get an available index to place the new Node at.
    const uint64 NEW_NODE_IDX = TUNDRA_INT_FUNC_NAME(get_avail_index)(list);

    // Fetch an available index and get a pointer to the Node at that index.
    TUNDRA_NODE_NAME *new_node = &list->nodes[NEW_NODE_IDX];

    // Copy the element into the new Node.
    #if TUNDRA_NEEDS_CUSTOM_COPY

        TUNDRA_COPY_CALL_SIG(elem, &new_node->datum);
    #else

        new_node->datum = *elem;
    #endif

    // Update the new Node to point next to the insert Node.
    new_node->next = IDX_OF_NODE_AT_INSERT_POS;
 
    // Update the new Node to point back to what the insert Node is pointing 
    // back to.
    new_node->prev = node_at_insert_idx->prev;

    // Update the Node before the insert Node to point next to the new Node.
    list->nodes[node_at_insert_idx->prev].next = NEW_NODE_IDX;
    
    // Update the insert Node to point back at the new Node
    node_at_insert_idx->prev = NEW_NODE_IDX;
    
    ++list->num_node;    
}

/**
 * @brief Resizes the List to contain `num_elem` elements.
 * 
 * If `num_elem` is the same as the number of elements in the List, nothing
 * is done.
 * 
 * If `num_elem` is greater than the current capacity, the Array's capacity is 
 * expanded exponentially to hold at least the new number of elements and the 
 * number of elements expands to `num_elem`. New elements are left 
 * untouched/uninitialized, but are indexable. Users are responsible for any 
 * initialization of the new elements.
 * 
 * If `num_elem` is less than the current number of elements, excess elements 
 * are discarded with the capacity remaining unchanged. If you wish to shrink 
 * the capacity, use `shrink_to_fit` or `shrink_to_new_cap`.
 * 
 * @param list List to resize.
 * @param num_elem Numer of elements to resize for.
 */
static inline void TUNDRA_FUNC_NAME(resize)(TUNDRA_LIST_NAME *list, 
    uint64 num_elem)
{
    if(num_elem == list->num_node) { return; }

    num_elem < list->num_node ? 
        TUNDRA_INT_FUNC_NAME(resize_smaller)(list, num_elem) :
        TUNDRA_INT_FUNC_NAME(resize_larger)(list, num_elem);

    // `num_node` does not track the Sentinel.
    list->num_node = num_elem;
}

/**
 * @brief Ensures the List has the capacity to store `num_extra_elems`. If it 
 * does not, expands and reallocates the List to hold the additional elements. 
 * 
 * The new expanded capacity will be the smallest power of 2 capable of holding 
 * the total needed elements.
 * 
 * @param list List to reserve for.
 * @param num_extra_elem Number of extra elements.
 */
static inline void TUNDRA_FUNC_NAME(reserve)(TUNDRA_LIST_NAME *list,
    uint64 num_extra_elem)
{
    // Subtract an additional 1 to account for the Sentinel, which is not 
    // accounted for in `num_node`.
    if(list->cap - list->num_node - 1 >= num_extra_elem) { return; }

    TUNDRA_INT_FUNC_NAME(reserve_for)(list, num_extra_elem);
}

/**
 * @brief Erases the element at the front of the List.
 * 
 * A fatal is thrown if the List is empty with the List unmodified.
 * 
 * @param list List to erase from.
 */
static inline void TUNDRA_FUNC_NAME(erase_front)(TUNDRA_LIST_NAME *list) 
{
    if(list->num_node == 0)
    {
        TUNDRA_FATAL("Attempted to erase but the List was empty.");
        return;
    }
   
    TUNDRA_NODE_NAME *sentinel_node = &list->nodes[TUNDRA_SENTINEL_IDX];
    
    const uint64 HEAD_NODE_IDX = sentinel_node->next;

    TUNDRA_NODE_NAME *node_to_free = &list->nodes[HEAD_NODE_IDX];

    // Datums need custom free handling.
    #if TUNDRA_NEEDS_CUSTOM_FREE

        TUNDRA_FREE_CALL_SIG(&node_to_free->datum);
    #endif

    // Update Sentinel to point to the Node after the head Node.
    sentinel_node->next = node_to_free->next;

    // Update the Node after the head Node to point back to the Sentinel.
    list->nodes[node_to_free->next].prev = TUNDRA_SENTINEL_IDX;

    // Add the erased index to the stack of freed indexes.
    Tundra_DynStku64_push(&list->freed_idxs, &HEAD_NODE_IDX);

    --list->num_node;
}

/**
 * @brief Erases the element at the back of the List.
 * 
 * A fatal is thrown if the List is empty with the List unmodified.
 * 
 * @param list List to erase from.
 */
static inline void TUNDRA_FUNC_NAME(erase_back)(TUNDRA_LIST_NAME *list) 
{
    if(list->num_node == 0)
    {
        TUNDRA_FATAL("Attempted to erase but the List was empty.");
        return;
    }

    TUNDRA_NODE_NAME *sentinel_node = &list->nodes[TUNDRA_SENTINEL_IDX];
    
    const uint64 TAIL_NODE_IDX = sentinel_node->prev;

    TUNDRA_NODE_NAME *node_to_free = &list->nodes[TAIL_NODE_IDX];

    // Datums need custom free handling
    #if TUNDRA_NEEDS_CUSTOM_FREE

        TUNDRA_FREE_CALL_SIG(&node_to_free->datum);
    #endif

    // Update the Sentinel to point ot the Node before the tial Node.
    sentinel_node->prev = node_to_free->prev;

    // Update hte Node before the tail Node to point next to the Sentinel.
    list->nodes[node_to_free->prev].next = TUNDRA_SENTINEL_IDX;

    // Add the erased index to the stack of freed indexes.
    Tundra_DynStku64_push(&list->freed_idxs, &TAIL_NODE_IDX);

    --list->num_node;
}

/**
 * @brief Erases the element at a specified index.
 * 
 * A fatal is thrown if the index is out of bounds with the List unmodified.
 * 
 * Must search linearly through the list, from either the start or the end to 
 * get to the element to erase.
 * 
 * @param list List to erase from.
 * @param index Index to erase.
 */
static inline void TUNDRA_FUNC_NAME(erase_at_index)(TUNDRA_LIST_NAME *list, 
    uint64 index)
{
    if(index >= list->num_node)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for List of size \"%llu\".", 
            index,  list->num_node);
        return;
    }

    TUNDRA_NODE_NAME *node_at_erase_idx = 
        TUNDRA_INT_FUNC_NAME(get_node_at_pos)(list, index);

    const uint64 IDX_OF_NODE_AT_ERASE_POS = 
        list->nodes[node_at_erase_idx->prev].next;

    #if TUNDRA_NEEDS_CUSTOM_FREE

        TUNDRA_FREE_CALL_SIG(&node_at_erase_idx->datum);
    #endif

    // Update the Node before the erase Node to point next to what the erase 
    // Node is pointing next to.
    list->nodes[node_at_erase_idx->prev].next = node_at_erase_idx->next;

    // Update the Node after the erase Node to point back to what the erase
    // Node is pointing back to.
    list->nodes[node_at_erase_idx->next].prev = node_at_erase_idx->prev;

    // Add the erased index to the stack of freed indexes.
    Tundra_DynStku64_push(&list->freed_idxs, &IDX_OF_NODE_AT_ERASE_POS);

    --list->num_node;
}

/**
 * @brief Returns a pointer to the element at `index` with bounds checking.
 * 
 * A fatal is thrown if the index is out of range with the List unmodified, 
 * returning NULL.
 * 
 * Must search linearly through the list, from either the start or the end to 
 * get to the element to return.
 * 
 * @param list List to index into.
 * @param index Index to erase.
 * 
 * @return TUNDRA_TYPE* Pointer to the element at the index.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(at)(TUNDRA_LIST_NAME *list, 
    uint64 index)
{
    if(index >= list->num_node)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for List of size \"%llu\".", 
            index, list->num_node);
        return NULL;
    }

    return &TUNDRA_INT_FUNC_NAME(get_node_at_pos)(list, index)->datum;
}

/**
 * @brief Returns a const-pointer to the element at `index` with bounds 
 * checking.
 * 
 * A fatal is thrown if the index is out of range with the List unmodified, 
 * returning NULL.
 * 
 * Must search linearly through the list, from either the start or the end to 
 * get to the element to return.
 * 
 * @param list List to index into.
 * @param index Index to erase.
 * 
 * @return const TUNDRA_TYPE* Pointer to the element at the index.
 */
static inline const TUNDRA_TYPE* TUNDRA_FUNC_NAME(at_cst)(
    const TUNDRA_LIST_NAME *list, uint64 index)
{
    if(index >= list->num_node)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for List of size \"%llu\".", 
            index, list->num_node);
        return NULL;
    }

    return &TUNDRA_INT_FUNC_NAME(get_node_at_pos_cst)(list, index)->datum;
}

/**
 * @brief Returns a pointer to the first element of the List.
 * 
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list List to query.
 * 
 * #return TUNDRA_TYPE* Pointer to the first element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front)(TUNDRA_LIST_NAME *list)
{
    return &list->nodes[TUNDRA_HEAD_IDX_EXPR].datum;
}

/**
 * @brief Returns a const-pointer to the first element of the List.
 * 
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list List to query.
 * 
 * #return const TUNDRA_TYPE* Const-pointer to the first element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(front_cst)(
    const TUNDRA_LIST_NAME *list)
{
    return &list->nodes[TUNDRA_HEAD_IDX_EXPR].datum;
}

/**
 * @brief Returns a pointer to the last element of the List.
 * 
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list List to query.
 * 
 * #return TUNDRA_TYPE* Pointer to the last element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(back)(TUNDRA_LIST_NAME *list)
{
    return &list->nodes[TUNDRA_TAIL_IDX_EXPR].datum;
}

/**
 * @brief Returns a const-pointer to the last element of the List.
 * 
 * @attention For fast access, this method does not perform a check if the List
 * is empty. It is the user's responsibility to ensure the List is not empty.
 * 
 * @param list List to query.
 * 
 * #return const TUNDRA_TYPE* Const-pointer to the last element.
 */
static inline TUNDRA_TYPE* TUNDRA_FUNC_NAME(back_cst)(
    const TUNDRA_LIST_NAME *list)
{
    return &list->nodes[TUNDRA_TAIL_IDX_EXPR].datum;
}

/**
 * @brief Returns the number of elements in the List.
 * 
 * @param list List to query.
 * 
 * @return uint64 Number of elements.
 */
static inline uint64 TUNDRA_FUNC_NAME(size)(const TUNDRA_LIST_NAME *list)
{
    return list->num_node;
}
/**
 * @brief Returns the current capacity of the List.
 * 
 * @param list List to query.
 * 
 * @param uint64 Current capacity.
 * 
 */
static inline uint64 TUNDRA_FUNC_NAME(capacity)(const TUNDRA_LIST_NAME *list)
{
    return list->cap;
}


// Iterator Methods ------------------------------------------------------------

/**
 * @brief Returns an iterator to the beginning of the List.
 * 
 * @param list List to get iterator for.
 * 
 * @return Tundra_LinkedListIteratorTYPE Iterator to the beginning of the List.
 */
static inline TUNDRA_ITER_NAME TUNDRA_ITER_FUNC_NAME(begin)(
    TUNDRA_LIST_NAME *list)
{
    return (TUNDRA_ITER_NAME)
    {
        .list = list,
        .index = TUNDRA_HEAD_IDX_EXPR
    };
}

/**
 * @brief Returns an iterator to one past the last element of the List.
 * 
 * This iterator must not be dereferenced.
 * 
 * @param list List to get iterator of.
 * 
 * @return Tundra_LinkedListIteratorTYPE Iterator to one past the last element.
 */
static inline TUNDRA_ITER_NAME TUNDRA_ITER_FUNC_NAME(end)(
    TUNDRA_LIST_NAME *list)
{
    return (TUNDRA_ITER_NAME)
    {
        .list = list,
        .index = TUNDRA_SENTINEL_IDX
    };
}

/**
 * @brief Returns an iterator to the element at an index.
 * 
 * A fatal is thrown if the index is invalid with an end Iterator returned.
 * 
 * Must search linearly through the list, from either the start or the end to 
 * get to the index into the list to get the Iterator of.
 * 
 * @param list List to get iterator of.
 * 
 * @return Tundra_LinkedListIteratorTYPE Iterator to an index.
 */
static inline TUNDRA_ITER_NAME TUNDRA_ITER_FUNC_NAME(get_at_index)(
    TUNDRA_LIST_NAME *list, uint64 index)
{
    if(index >= list->num_node)
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for List of size \"%llu\".", 
            index, list->num_node);

        return TUNDRA_ITER_FUNC_NAME(end)(list);
    }

    return (TUNDRA_ITER_NAME)
    {
        .list = list,
        .index = TUNDRA_INT_FUNC_NAME(find_idx_of_node)(list, index)
    };
}

/**
 * @brief Returns true if both iterators point to the same index.
 * 
 * Assumes that the iterators come from the same List. This means that if the 
 * iterators are from different Lists but have the same pointed to index, this
 * method returns true. Only compare iterators from the same List.
 * 
 * @param first First iterator.
 * @param second Second iterator.
 * 
 * @return bool True if both iterators point to the same index.
 */
static inline bool TUNDRA_ITER_FUNC_NAME(compare)(const TUNDRA_ITER_NAME *first, 
    TUNDRA_ITER_NAME *second)
{
    return first->index == second->index;
}

/**
 * @brief Moves an iterator to the next index.
 * 
 * Does not check for going past the end iterator.
 * 
 * @param iter Iterator to advance.
 */
static inline void TUNDRA_ITER_FUNC_NAME(next)(TUNDRA_ITER_NAME *iter)
{
    iter->index = iter->list->nodes[iter->index].next;
}

/**
 * @brief Moves an iterator to the previous index.
 * 
 * Does not check for going past the end iterator.
 * 
 * @param iter Iterator to move back.
 */
static inline void TUNDRA_ITER_FUNC_NAME(prev)(TUNDRA_ITER_NAME *iter)
{
    iter->index = iter->list->nodes[iter->index].prev;
}

/**
 * @brief Dereferences an iterator to get a pointer to the current element.
 * 
 * Does not check if the iterator is valid.
 * 
 * @param iter Iterator to deref.
 * 
 * @return TYPE* Pointer to the current element.
 */
static inline TUNDRA_TYPE* TUNDRA_ITER_FUNC_NAME(deref)(
    const TUNDRA_ITER_NAME *iter)
{
    return &iter->list->nodes[iter->index].datum;
}

/**
 * @brief Dereferences an iterator to get a const-pointer to the current 
 * element.
 * 
 * Does not check if the iterator is valid.
 * 
 * @param iter Iterator to dereference.
 * 
 * @return const TYPE* Const-pointer to the current element.
 */
static inline const TUNDRA_TYPE* TUNDRA_ITER_FUNC_NAME(deref_cst)(
    const TUNDRA_ITER_NAME *iter)
{
    return &iter->list->nodes[iter->index].datum;
}

#ifdef __cplusplus
} // Extern "C"
#endif


#undef TUNDRA_LIST_NAME
#undef TUNDRA_NODE_NAME
#undef TUNDRA_ITER_NAME
#undef TUNDRA_FUNC_NAME
#undef TUNDRA_INT_FUNC_NAME
#undef TUNDRA_ITER_FUNC_NAME
#undef TUNDRA_MAX_ELEMS_NAME
#undef TUNDRA_NODE_SIZE
#undef TUNDRA_HEAD_IDX_EXPR;
#undef TUNDRA_TAIL_IDX_EXPR;
#undef TUNDRA_SENTINEL_IDX;
