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


// Internal Methods ------------------------------------------------------------

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

    // Initialize Sentinel. Points to itself since the List is empty.
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;
    // `prev` member of the Sentinel is set with the `update_tail_idx` call.

    list->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    TUNDRA_INT_FUNC_NAME(update_tail_idx)(list, TUNDRA_LNKLST_SENTINEL_IDX);

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
    // Start at the first Node at the head index.
    const TUNDRA_NODE_NAME *src_parsed_node = src->nodes + src->head_idx;
    TUNDRA_NODE_NAME *dst_parsed_node = dst->nodes + dst->head_idx;

    while(src_parsed_node != src->nodes + TUNDRA_LNKLST_SENTINEL_IDX)
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
    while(parsed_node != list->nodes + TUNDRA_LNKLST_SENTINEL_IDX)
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

    // -- Initialize Sentinel in the new mem -- 
    
    // Sentinel Node pointers next to the head Node, which will be placed 1 in 
    // front of it.
    new_mem[TUNDRA_LNKLST_SENTINEL_IDX].next = 
        TUNDRA_LNKLST_SENTINEL_IDX + 1;
    // `prev` member of the Sentinel is set on the `update_tail_idx` call below.

    // Current parsed Node into the old memory of the List.
    TUNDRA_NODE_NAME *src_parsed_node = &list->nodes[list->head_idx]; 

    // Index where the Head Node will be placed inside the new memory, 1 after
    // the Sentinel.
    enum { HEAD_NODE_IDX = TUNDRA_LNKLST_SENTINEL_IDX + 1 }; 

    // Current index into the new memory array. We start at 1 past the 
    // Sentinel index, since the Sentinel is reserved at the 0th index.
    uint64 i = HEAD_NODE_IDX;

    // While the parsed Node inside the old memory does not reach the Sentinel.
    while(src_parsed_node != list->nodes + TUNDRA_LNKLST_SENTINEL_IDX)
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
    list->nodes[TAIL_NODE_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;

    // Free the old memory. Since the custom move call would've handled the old 
    // Node datums if they required custom handling, we don't need to worry 
    // about iterating through each Node and custom freeing its datum.
    Tundra_free_mem((void*)list->nodes);

    list->nodes = new_mem;

    list->head_idx = HEAD_NODE_IDX;
    // Update tail index and Sentinel prev member.
    TUNDRA_INT_FUNC_NAME(update_tail_idx)(list, TAIL_NODE_IDX);
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
    // Add 1 to account for the sentinel.
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
static inline uint64 TUNDRA_INT_FUNC_NAME(get_avail_index)(TUNDRA_LIST_NAME *list)
{
    // If there are no available indexes that have been freed.
    if(Tundra_DynStku64_is_empty(&list->freed_idxs))
    {
        // Since there are no freed positions inside the array of Nodes, the 
        // first available position will always be at the end of the filled 
        // array of Nodes.
        return list->num_node;
    }

    // -- Fetch an available index from the stack of freed indexes. --

    const uint64 avail_index = *Tundra_DynStku64_front_cst(&list->freed_idxs);
    Tundra_DynStku64_pop(&list->freed_idxs);
    return avail_index;
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

    // Index where the head Node will be  placed, which is 1 after the Sentinel.
    enum { HEAD_NODE_IDX = TUNDRA_LNKLST_SENTINEL_IDX + 1 };

    // -- Initialize Sentinel Node --

    // Sentinel Node points next to the head Node.
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].next = HEAD_NODE_IDX;
    // `prev` member of the Sentinel is set on the `update_tail_idx` call below.

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
    list->nodes[TAIL_NODE_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;
    
    list->num_node = num_elem;
    list->head_idx = HEAD_NODE_IDX;
    // Update tail index and Sentinel prev member.
    TUNDRA_INT_FUNC_NAME(update_tail_idx)(list, list->num_node);
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
    src->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    src->tail_idx = TUNDRA_LNKLST_SENTINEL_IDX;
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
    list->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    list->tail_idx = TUNDRA_LNKLST_SENTINEL_IDX;
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

    dst->head_idx = src->head_idx;
    dst->tail_idx = src->tail_idx;
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
    src->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    src->tail_idx = TUNDRA_LNKLST_SENTINEL_IDX;
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
    list->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    list->tail_idx = TUNDRA_LNKLST_SENTINEL_IDX;
}

// /**
//  * @brief Adds a copy of an element to the front of the List.
//  * 
//  * @param list List to add to.
//  * @param elem Pointer to the element to add.
//  */
// static inline void TUNDRA_FUNC_NAME(add_front)(TUNDRA_LIST_NAME *list, 
//     const TUNDRA_TYPE *elem)
// {
//     TUNDRA_INT_FUNC_NAME(check_handle_exp)(list);

//     const uint64 ADD_IDX = TUNDRA_INT_FUNC_NAME(get_avail_index)(list);

    
// }

#ifdef __cplusplus
} // Extern "C"
#endif


#undef  TUNDRA_MAX_ELEMS_NAME
#undef TUNDRA_LIST_NAME
#undef TUNDRA_NODE_NAME
#undef TUNDRA_ITER_NAME
#undef TUNDRA_FUNC_NAME
#undef TUNDRA_INT_FUNC_NAME
#undef TUNDRA_ITER_FUNC_NAME
#undef TUNDRA_NODE_SIZE
