/**
 * @file LinkedList.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Dynamically linked container for efficiently inserting and removing
 * elements.
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 */

/**
 * To define a DynamicArray type, define the following macros as well as a 
 * header guard before including this file:
 * - TUNDRA_TYPE: Type the List will store.
 * If these macros are not defined, they will default to:
 * - TUNDRA_TYPE: int
 * 
 * Additionally, the DynamicStackuint64 type must be defined before including 
 * this file since it is used internally by the LinkedList.
 */

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_LNKLST_H
#define TUNDRA_LNKLST_H
#define TUNDRA_LNKLST_DEF_CAP 4
#define TUNDRA_LNKLST_SENTINEL_IDX 0
#endif

#ifndef TUNDRA_LIST_TYPE
#define TYPE int
#else
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

#ifndef TUNDRA_DYNSTK_UINT64
#error "Define DynamicStackuint64 before including LinkedList.h"
// Define needed components to silence errors.
#define Tundra_DynamicStackuint64
#define Tundra_DynStkuint64_init(stack)
#define Tundra_DynStkuint64_free(stack)
#define Tundra_DynStkuint64_copy(src, dst)
#endif

#define LIST_NAME TUNDRA_CONCAT(Tundra_LinkedList, TYPE)
#define NODE_NAME TUNDRA_CONCAT(Tundra_LnkLstNode, TYPE)
#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_LnkLst, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_LnkLst, TYPE, _##name)

#define TUNDRA_LNKLST_NODE_SIZE sizeof(TUNDRA_EXPAND(NODE_NAME))

#ifdef __cplusplus
extern "C" {
#endif

// Containers ------------------------------------------------------------------


typedef struct NODE_NAME
{
    TYPE datum;

    // Index of the next node, or SENTINEL_IDX if there isn't one.
    uint64 next;

    // Index of the previous node, or SENTINEL_IDX if there isn't one.
    uint64 prev;

} NODE_NAME;


typedef struct LIST_NAME
{
    // Array of nodes.
    NODE_NAME *nodes;

    // Index of the head Node, or SENTINEL_IDX if there isn't one.
    uint64 head_idx;
    
    // Index of the tail Node, or SENTINEL_IDX if there isn't one.
    uint64 tail_idx;

    // Number of Nodes in the List.
    uint64 num_node;

    // Current maximum number of elements that can be added to the List before
    // it needs to be resized.
    uint64 cap;

    // Indexes inside the underlying array of Nodes that have been freed and 
    // can be reused.
    struct Tundra_DynamicStackuint64 freed_idx; 

    // Copy function invoked when the `copy` method is called. 
    void (*copy_func)(const TYPE*, TYPE*);
    
    // Free function invoked when the `free` method is called.
    
    // Free method 
    void (*free_func)(LIST_NAME*);

} LIST_NAME;


// Internal Methods -----------------------------------------------------------

/**
 * @brief Updates the tail index of a List.
 * 
 * Also updates the sentinel's `prev` member to match the new tail index.
 * 
 * @param list List to update.
 * @param idx New tail index.
 */
static inline void INT_FUNC_NAME(update_tail_idx)(LIST_NAME *list, uint64 idx)
{
	list->tail_idx = idx;
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].prev = idx;
}

static inline void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst)
{
    // Simple byte copy
    *dst = *src;
}

static inline void INT_FUNC_NAME(def_free)(LIST_NAME *list)
{
    // Simply free Nodes array, does not touch the elements themselves.
    Tundra_free_mem(list->nodes);
    list->nodes = NULL;
}

static inline void INT_FUNC_NAME(init)(LIST_NAME *list, uint64 init_cap)
{
    Tundra_DynStkuint64_init(&list->freed_idx);

    list->copy_func = INT_FUNC_NAME(def_copy);
    list->free_func = INT_FUNC_NAME(def_free);        

    // Allocate Nodes array
    list->nodes = (NODE_NAME*)Tundra_alloc_mem(
        init_cap * TUNDRA_LNKLST_NODE_SIZE);

    // Initialize Sentinel. Points to itself, no next.
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;
    // `prev` member of the sentinel is set with the `update_tail_idx` call.

    list->head_idx = TUNDRA_LNKLST_SENTINEL_IDX;
    INT_FUNC_NAME(update_tail_idx)(list, TUNDRA_LNKLST_SENTINEL_IDX);

    list->num_node = 0;
    list->cap = init_cap;
}


// Public Methods -------------------------------------------------------------

/**
 * @brief Initializes a List with default capacity. Allocates memory and sets
 * internal components.
 * 
 * Only initialize a List once. If an already initialized List is called with
 * init, undefined behavior may occur.
 * 
 * @param list List to init. 
 */
static inline void FUNC_NAME(init)(LIST_NAME *list)
{
   INT_FUNC_NAME(init)(list, TUNDRA_LNKLST_DEF_CAP); 
}

/**
 * @brief Initializes a List with a set capacity. Allocates memory and sets 
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
static inline void FUNC_NAME(init_w_cap)(LIST_NAME *list, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_LNKLST_DEF_CAP : init_cap; 

    INT_FUNC_NAME(init)(list, init_cap);
}

static inline void FUNC_NAME(init_w_elems)(LIST_NAME *list, 
    const TYPE *elements, uint64 num_elem, bool strict_alloc)
{
    if(num_elem == 0)
    {
        TUNDRA_FATAL("Attempted to initialize list but initial number of "
            "elements is 0.");
        return;
    }

    list->copy_func = INT_FUNC_NAME(def_copy);
    list->free_func = INT_FUNC_NAME(def_free);

    Tundra_DynStkuint64_init(&list->freed_idx);

    // Number of elements needed for allocating with an extra space in mind for
    // the sentinel.
    const uint64 NUM_ELEM_W_SENT = num_elem + 1;
    const uint64 NUM_RESERVE_BYTE = 
        NUM_ELEM_W_SENT * sizeof(NODE_NAME);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        list->nodes = (NODE_NAME*)Tundra_alloc_mem(NUM_RESERVE_BYTE);
        list->cap = NUM_ELEM_W_SENT;
    }

    // Use the "reserving method" to alloc.
    else
    {
        uint64 temp_cap_bytes;

        Tundra_alloc_reserve_mem(
            (void**)(&list->nodes),
            &temp_cap_bytes,
            NUM_RESERVE_BYTE
        );

        list->cap = temp_cap_bytes / sizeof(NODE_NAME);
    }

    const uint64 FIRST_NODE_IDX = 1; // Index 0 is reserved for sentinel.

    // -- Initialize Sentinel --

    // Sentinel points next to itself, no next.
    list->nodes[TUNDRA_LNKLST_SENTINEL_IDX].next = TUNDRA_LNKLST_SENTINEL_IDX;
    // `prev` member of the sentinel is set with the `update_tail_idx` call.

    // -- Place first Node --

    // Place the first Node since it doesn't need to reset the `next` member of 
    // the Node before it.
    list->nodes[FIRST_NODE_IDX].datum = elements[0];
    list->nodes[FIRST_NODE_IDX].prev = TUNDRA_LNKLST_SENTINEL_IDX;

    // -- Place the rest of the Nodes --

    // Start at 1 past the first Node index since we've added the initial Node.
    for(uint64 i = FIRST_NODE_IDX + 1; i < num_elem + 1; ++i)
    {
        // Use i - 1 for the index into elements since elements does not have 
        // the sentinel offset.
        list->nodes[i - 1].next = i;
        list->nodes[i].prev = i - 1;
        list->nodes[i].datum = elements[i - 1];
    }

    // Set the last added Node's next value since it does not have a next Node.
    // Use `num_elem` straight here since we're ahead by 1 in the List compared
    // to the initial elements array due to the sentinel.
    list->nodes[num_elem].next = TUNDRA_LNKLST_SENTINEL_IDX;

    // -- Set container components --
    list->head_idx = FIRST_NODE_IDX;
    INT_FUNC_NAME(update_tail_idx)(list, num_elem);
    list->num_node = num_elem;
}

/**
 * @brief Frees memory allocated for a List.
 * 
 * After calling this method, the List must not be used unless reinitialized.
 *
 * It is safe to call this method on a List that has already been freed, or 
 * never initialized.
 * 
 * @param list List to free.
 */
static inline void FUNC_NAME(free)(LIST_NAME *list)
{
    Tundra_DynStkuint64_free(&list->freed_idx);
    list->free_func(list);
}

static inline void FUNC_NAME(copy)(const LIST_NAME *src, LIST_NAME *dst)
{
    if(src == dst) { return; }

    Tundra_DynStkuint64_copy(&src->freed_idx, &dst->freed_idx);

    // if(dst->cap != src->cap)
}



#ifdef __cplusplus
} // extern "C"
#endif


#undef TYPE
#undef LIST_NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
