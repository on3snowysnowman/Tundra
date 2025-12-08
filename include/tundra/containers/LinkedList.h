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

#ifndef Tundra_DynamicStack_uint64
#error "Define DynamicStack_uint64 before including LinkedList.h"
// Define needed components to silence errors.
#define Tundra_DynamicStack_uint64
#define Tundra_DynStk_init_uint64(stack)
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
    struct Tundra_DynamicStack_uint64 freed_idx; 

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
    Tundra_DynStk_init_uint64(&list->freed_idx);

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


#ifdef __cplusplus
} // extern "C"
#endif


#undef TYPE
#undef LIST_NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
