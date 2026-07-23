/**
 * @file HashTable.h
 * @author your name (you@domain.com)
 * @brief Key-value container providing constant-time access.
 * @version 0.1
 * @date 2026-07-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_HASHTABLE_H
#define TUNDRA_HASHTABLE_H
#define TUNDRA_HSHTBL_DEF_CAP 16

// Ratio (out of 10) of used/capacity elements in the top of the table, where if
// exceeded will be expanded and rehashed. 
#define TUNDRA_HSHTBL_TOP_LIMIT 7

// Proportion (out of 10) of the total table capacity that will be allocated for
// the top of the table, where the remaining space is reserved for the cellar.
#define TUNDRA_HSHTBL_TOP_PROPORTION 8
#endif

#include "tundra/common/TypeDef.h"
#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TUNDRA_KEY
#error TUNDRA_KEY not defined.
#endif

#ifndef TUNDRA_KEYNAME
#error TUNDRA_KEYNAME not defined.
#endif

#ifndef TUNDRA_VAL
#error TUNDRA_VAL not defined.
#endif

#ifndef TUNDRA_VALNAME
#error TUNDRA_VALNAME not defined.
#endif

#ifndef TUNDRA_HASH_FUNC
#error TUNDRA_HASH_FUNC not defined.
#endif

// Type and Function Name Macros -----------------------------------------------
#define TUNDRA_TBL_NAME TUNDRA_CONCAT3(Tundra_HashTable, TUNDRA_KEYNAME, \
    TUNDRA_VALNAME)
#define TUNDRA_ENTRY_NAME TUNDRA_CONCAT3(Tundra_HshTblEntry, TUNDRA_KEYNAME, \
    TUNDRA_VALNAME)

#define TUNDRA_FUNC_NAME(name) TUNDRA_CONCAT4(Tundra_HshTbl, TUNDRA_KEYNAME, \
    TUNDRA_VALNAME, _##name)
#define TUNDRA_INT_FUNC_NAME(name) TUNDRA_CONCAT4(InTundra_HshTbl, \
    TUNDRA_KEYNAME, TUNDRA_VALNAME, _##name)

// Containers ------------------------------------------------------------------

typedef struct
{
    TUNDRA_KEY key;
    TUNDRA_VAL val;

    // If this integer is greater than 0, it represents the index inside the 
    // cellar of the next Entry in the collision chain. If this int is -2, the
    // Entry is non initialized/used and a new Entry can be placed here. If -1,
    // this Entry is valid and holds an added key/value pair, and there is no
    // collision chain attached to this Entry yet since the status is not 
    // greater than 0.
    i64 status;
} TUNDRA_ENTRY_NAME;

typedef struct
{
    // Array of all Entrys (including the Cellar at the end).
    TUNDRA_ENTRY_NAME *data;
    
    // Number of valid Entrys in the top of the table. 
    u64 num_entries_top;

    // Capacity in Entrys of the top of the table.
    u64 top_capacity; 
    
    // Capacity in Entrys of the cellar of the table.
    u64 cellar_capacity;

    // Index inside the cellar of the next empty position, when the 
    // `avail_cellar_idxs` stack is empty.
    u64 next_avail_cell_idx;

    // Indexes inside the cellar that have been freed and can be reused.
    Tundra_DynamicStackU64 avail_cellar_idxs;

} TUNDRA_TBL_NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Internal init method called by public init methods. Allocates initial 
 * memory for `init_cap` elems (split into top and cellar) and sets container
 * components.
 * 
 * Assumes `init_cap` is a power of 2.
 * 
 * @param tbl Table to initialize.
 * @param init_cap Initial capacity in elems (Top + Cellar)
 */
#include "tundra/utils/ConsoleIO.h"
static inline void TUNDRA_INT_FUNC_NAME(init)(TUNDRA_TBL_NAME *tbl, 
    u64 init_cap)
{
    tbl->num_entries_top = 0;
    tbl->top_capacity = 
        (u64)((float)init_cap * (TUNDRA_HSHTBL_TOP_PROPORTION / 10.0f));
    tbl->cellar_capacity = init_cap - tbl->top_capacity;
    tbl->next_avail_cell_idx = 0;

    Tundra_DynStkU64_init(&tbl->avail_cellar_idxs);
}


// Public ----------------------------------------------------------------------

static inline void TUNDRA_FUNC_NAME(init)(TUNDRA_TBL_NAME *tbl)
{
    TUNDRA_INT_FUNC_NAME(init)(tbl, TUNDRA_HSHTBL_DEF_CAP);
}

static inline void TUNDRA_FUNC_NAME(init_cap)(TUNDRA_TBL_NAME *tbl,
    u64 init_cap)
{
    if(init_cap == 0)
    {
        TUNDRA_FATAL("init_cap cannot be 0.\n");
        return;
    }

    init_cap = Tundra_ceil_pow2(init_cap);
    
    TUNDRA_INT_FUNC_NAME(init)(tbl, init_cap);
}

static inline void TUNDRA_FUNC_NAME(init_copy)(TUNDRA_TBL_NAME *dst, 
    const TUNDRA_TBL_NAME *src)
{
    // Shallow copy initially, we will deep copy required members next. 
    *dst = *src;

    
}


static inline void TUNDRA_FUNC_NAME(free)(TUNDRA_TBL_NAME *tbl)
{
    Tundra_free_mem(tbl->data);
    tbl->data = NULL;

    tbl->num_entries_top = 0;
    tbl->top_capacity = 0;
    tbl->cellar_capacity = 0;
    tbl->next_avail_cell_idx = 0;

    Tundra_DynStkU64_free(&tbl->avail_cellar_idxs);
}


#ifdef __cplusplus
}
#endif
