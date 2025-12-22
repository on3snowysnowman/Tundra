/**
 * @file DynamicStack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container providing LIFO behavior for storing
 * procedurally added elements.
 * @date 2025-12-06
 * 
 * @copyright Copyright (c) 2025
 */

/**
 * To define a DynamicStack type, define the following macros as well as a 
 * header guard before including this file:
 * - TUNDRA_TYPE: Type the Stack will store.
 * If these macros are not defined, they will default to:
 * - TUNDRA_TYPE: int
*/

#include "tundra/utils/CoreDef.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/utils/Math.h"

#ifndef TUNDRA_DYNSTK_H
#define TUNDRA_DYNSTK_H
#define TUNDRA_DYNSTK_DEF_CAP 4
#endif

#ifndef TUNDRA_TYPE
#define TYPE int
#else 
#define TYPE TUNDRA_EXPAND(TUNDRA_TYPE)
#endif

#define NAME TUNDRA_CONCAT(Tundra_DynamicStack, TYPE)
#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynStk, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynStk, TYPE, _##name)


#ifdef __cplusplus
extern "C" {
#endif

// Containers ------------------------------------------------------------------

typedef struct NAME
{
    // Heap allocated array of elements on the Stack.
    TYPE *data;

    // Number of elements on the Stack.
    uint64 num_elem;

    // Current maximum number of elements that can be pushed on the Stack before
    // it needs to be resized.
    uint64 cap;

    // Copy function invoked when the `copy` method is called.
    void (*copy_func)(const TYPE *src, TYPE *dst, uint64 num_elem);

    // Free function invoked when the `free` method is called.
    void (*free_func)(TYPE *mem, uint64 num_elem);
} NAME;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Default copy method, performs simple byte copy on `num_elem` elements
 * from `src` to `dst`.
 * 
 * @param src Stack of elements to source from.
 * @param dst Stack of elements to copy to.
 * @param num_elem Number of elements to copy.
 */
static inline void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, uint64 
    num_elem)
{
    Tundra_copy_mem_fwd((const void*)(src), (void*)(dst),
        num_elem * sizeof(TYPE));
}

/**
 * @brief Default free method that simply frees the memory.
 * 
 * @param mem Mem to free.
 */
static inline void INT_FUNC_NAME(def_free)(TYPE *mem, uint64 /** num_elem */)
{
    Tundra_free_mem((void*)(mem));
}

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` elements and sets container components.
 * 
 * @param stk Stack to initialize.
 * @param init_cap Initial capacity in elements.
 */
static inline void INT_FUNC_NAME(init)(NAME *stk, uint64 init_cap)
{
    stk->data = (TYPE*)(Tundra_alloc_mem(init_cap * sizeof(TYPE)));

    stk->num_elem = 0;
    stk->cap = init_cap;
    stk->copy_func = INT_FUNC_NAME(def_copy);
    stk->free_func = INT_FUNC_NAME(def_free);
}

/**
 * @brief Checks if an Stack has filled its allocated capacity, expanding and
 * reallocating if it has.
 * 
 * @param stk Stack to handle. 
 */
static inline void INT_FUNC_NAME(check_handle_exp)(NAME *stk)
{
    if(stk->num_elem < stk->cap) { return; }

    // Double previous capacity.
    const uint64 NEW_CAP_ELEM = 2 * stk->cap;

    TYPE *new_mem = (TYPE*)(Tundra_alloc_mem(NEW_CAP_ELEM * sizeof(TYPE)));

    // Copy data from the Stack to the new memory.
    stk->copy_func(stk->data, new_mem, stk->num_elem);
    stk->free_func(stk->data, stk->num_elem);
    stk->data = new_mem;
    stk->cap = NEW_CAP_ELEM;
}

/**
 * @brief Expands the Stack to ensure it has the capacity to store 
 * `num_extra_elem` elements.
 * 
 * Assumes that the current number of elements plus `num_extra_elem` exceeds the
 * current capacity.
 * 
 * @param stk Stack to handle. 
 * @param num_extra_elem Number of extra elements.
 */
static inline void INT_FUNC_NAME(reserve_for)(NAME *stk, uint64 num_extra_elem)
{
    const uint64 TOT_REQ_BYTE = 
        (stk->num_elem + num_extra_elem) * sizeof(TYPE);

    // Calculate new capacity by doubling current capacity until the required
    // bytes are reached.
    const uint64 NEW_CAP_BYTE = InTundra_calc_new_capacity_by_doubling(
        TOT_REQ_BYTE, stk->cap * sizeof(TYPE));

    // Allocate new memory and copy existing elements over.
    TYPE *new_mem = (TYPE*)(Tundra_alloc_copy_mem(
        (const void*)stk->data,
        NEW_CAP_BYTE,
        stk->cap * sizeof(TYPE)
    ));

    // Free old memory.
    stk->free_func(stk->data, stk->num_elem);
    stk->data = new_mem;
    stk->cap = NEW_CAP_BYTE / sizeof(TYPE);
}

/**
 * @brief Internal shrink method, reallocates the Stack to a capacity of `cap`.
 * 
 * @param stk Stack to shrink. 
 * @param cap Capacity to shrink to.
 */
static inline void INT_FUNC_NAME(shrink)(NAME *stk, uint64 cap)
{
    const uint64 CAP_BYTE = cap * sizeof(TYPE);

    TYPE *new_mem = (TYPE*)Tundra_alloc_copy_mem(stk->data, CAP_BYTE, CAP_BYTE);

    // Free old memory.
    stk->free_func(stk->data, stk->num_elem);
    stk->data = new_mem;
    stk->cap = cap;

    // If the capacity was shrunk smaller than the existing elements, update the
    // number of elements.
    stk->num_elem = Tundra_clamp_max_u64(stk->num_elem, stk->cap);
}

// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes a Stack with default capacity. Allocates memory and 
 * sets internal components.
 *
 * Only initialize an Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 * 
 * @param stk Stack to init. 
*/
static inline void FUNC_NAME(init)(NAME *stk)
{
    INT_FUNC_NAME(init)(stk, TUNDRA_DYNSTK_DEF_CAP);
}

/**
 * @brief Initialize a Stack with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Stack is initialized with default capacity.
 *
 * Only initialize a Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 * 
 * @param stk Stack to init, 
 * @param init_cap Specified initial capacity.
 */
static inline void FUNC_NAME(init_w_cap)(NAME *stk, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_DYNSTK_DEF_CAP : init_cap; 

    INT_FUNC_NAME(init)(stk, init_cap);
}

/**
 * @brief Initializes a Stack with initial elements. Allocates memory and sets
 * internal components.
 *
 * `elements` are copied into the Stack. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated for the Stack. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 * 
 * The last element in `elements` will be the element on the top of the Stack.
 * 
 * Only initialize a Stack once. If an already initialized Stack is called with
 * init, undefined behavior may occur. 
 *
 * @param stk Stack to init. 
 * @param elements Stack of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
static inline void FUNC_NAME(init_w_elems)(NAME *stk, const TYPE *elements, 
    uint64 num_elem, bool strict_alloc)
{
    stk->copy_func = INT_FUNC_NAME(def_copy);
    stk->free_func = INT_FUNC_NAME(def_free);

    const uint64 NUM_CPY_BYTE = num_elem * sizeof(TYPE);

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        stk->data = (TYPE*)(Tundra_alloc_mem(NUM_CPY_BYTE));
        stk->copy_func(elements, stk->data, num_elem);
        stk->num_elem = num_elem;
        stk->cap = num_elem;
        return;
    }
    
    // -- Use the "reserving" method to alloc, which will generally alloc more
    // space than is needed to prevent immediate expansion on next push call. --
    // Temp var for retrieving the capacity of the allocated block through the
    // next call. Capacity in bytes, not elements.
    uint64 temp_cap_bytes = 0;
    Tundra_alloc_reserve_mem(
        (void**)(&stk->data), 
        &temp_cap_bytes,
        NUM_CPY_BYTE
    );

    stk->copy_func(elements, stk->data, num_elem);
    stk->num_elem = num_elem;
    stk->cap = temp_cap_bytes / sizeof(TYPE);
}

/**
 * @brief Sets the copy function used when copying elements.
 * 
 * @param stk Stack to set copy function for.
 * @param copy_func Copy function to set.
 */
static inline void FUNC_NAME(set_copy_func)(NAME *stk, 
    void (*copy_func)(const TYPE*, TYPE*, uint64))
{
    stk->copy_func = copy_func;
}

/**
 * @brief Sets the free function used when freeing elements.
 * 
 * @param stk Stack to set free function for.
 * @param free_func Free function to set.
 */
static inline void FUNC_NAME(set_free_func)(NAME *stk, 
    void (*free_func)(TYPE*, uint64))
{
    stk->free_func = free_func;
}

/**
 * @brief Frees memory allocated for an Stack.
 *
 * After calling this method, the Stack must not be used unless reinitialized.
 *
 * It is safe to call this method with an Stack that has already been freed, or
 * never initialized.
 * 
 * @param stk Stack to free.
 */
static inline void FUNC_NAME(free)(NAME *stk)
{
    stk->free_func(stk->data, stk->num_elem);
    stk->data = NULL;
    stk->copy_func = NULL;
    stk->free_func = NULL;
}

/**
 * @brief Deep copies `src` to `dst`.
 *
 * If the Stacks are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Stack. 
 * 
 * @param src Stack to source from. 
 * @param dst Stack to deep copy to, can be uninitialized. 
 */
static inline void FUNC_NAME(copy)(const NAME *src, NAME *dst)
{
    if(dst == src) { return; }

    const uint64 SRC_CAP_BYTE = src->cap * sizeof(TYPE);

    if(dst->cap != src->cap)
    {
        src->free_func(dst->data, dst->num_elem);
        dst->data = (TYPE*)(Tundra_alloc_mem(SRC_CAP_BYTE));
        dst->cap = src->cap;
    }

    src->copy_func(src->data, dst->data, src->num_elem);
    dst->num_elem = src->num_elem;
    dst->copy_func = src->copy_func;
    dst->free_func = src->free_func;
}

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.
 *
 * If the Stacks are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized Stack.
 * 
 * @param src Stack to source from. 
 * @param dst Stack to transfer resources to, can be uninitialized.
 */
static inline void FUNC_NAME(move)(NAME *src, NAME *dst)
{
    if(dst == src) { return; }
  
    src->free_func(dst->data, dst->num_elem);
    *dst = *src;
    src->data = NULL;
    src->copy_func = NULL;
    src->free_func = NULL;
}

/**
 * @brief Clears the Stack of all elements.
 *
 * Does not modify, shrink, deallocate or zero out underlying memory. Only the 
 * element count is reset to zero so subsequent adds will overwrite previous 
 * data from the top of the Stack.
 * 
 * @param stk Stack to clear. 
 */
static inline void FUNC_NAME(clear)(NAME *stk)
{
    stk->num_elem = 0;
}

/**
 * @brief Pushes a copy of `elem` onto the Stack, automatically resizing if 
 * needed.
 * 
 * @param stk Stack to push to.
 * @param elem Element to copy.
 */
static inline void FUNC_NAME(push)(NAME *stk, TYPE elem)
{
    INT_FUNC_NAME(check_handle_exp)(stk);

    stk->data[stk->num_elem++] = elem;
}

/**
 * @brief Pops a value off the Stack.
 *
 * Performs a size check to ensure the Stack contains elements before popping.
 * A fatal is thrown if the Stack contains no elements with the Stack 
 * unmodified.
 *
 * Does not return the popped value. Call the `top` method to get a reference to 
 * the last element before popping.
 * 
 * @param stk Stack to pop.
*/
static inline void FUNC_NAME(pop)(NAME *stk)
{
    if(stk->num_elem == 0)
    {
        TUNDRA_FATAL("Attempted to pop from an empty Stack.");
        return;
    }

    --stk->num_elem;
}

/**
 * @brief Expands the Stack to ensure it has the capacity to store 
 * `num_extra_elem`additional elements.
 * 
 * If the Stack already has enough capacity, nothing is done.
 * 
 * @param stk Stack to reserve for. 
 * @param num_extra_elem Number of extra elements.
 */
static inline void FUNC_NAME(reserve)(NAME *stk, uint64 num_extra_elem)
{
    if(stk->num_elem + num_extra_elem <= stk->cap) { return; }

    INT_FUNC_NAME(reserve_for)(stk, num_extra_elem);
}

/**
 * @brief Shrinks the Stack's allocated capacity to a specified capacity.
 *
 * If `new_cap` is greater than or equal to the current capacity, the Stack is
 * not modified. If `new_cap` is less than the current number of elements, 
 * excess elements are discarded and the Stack is resized to the value 
 * specified.
 *
 * Memory is reallocated if the capacity is reduced. 
 *
 * @param stk Stack to shrink. 
 * @param new_cap Capacity to shrink to.
 */
static inline void FUNC_NAME(shrink_to_new_cap)(NAME *stk, uint64 new_cap)
{
    if(new_cap >= stk->cap) { return; }

    INT_FUNC_NAME(shrink)(stk, new_cap);
}

/**
 * @brief Shrinks the Stack's allocated capacity to match its current number of 
 * elements.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param stk Stack to shrink.
 */
static inline void FUNC_NAME(shrink_to_fit)(NAME *stk)
{
    if(stk->num_elem == stk->cap) { return; }

    INT_FUNC_NAME(shrink)(stk, stk->num_elem);
}

/**
 * @brief Returns true if the Stack contains no elements.
 * 
 * @param stk Stack to check.
 * 
 * @return bool True if Stack is empty. 
 */
static inline bool FUNC_NAME(is_empty)(const NAME *stk)
{
    return stk->num_elem == 0;
}

/**
 * @brief Returns a pointer to the top element of the Stack.
 *
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Stack to query.
 * 
 * @return TYPE* Pointer to the top element.
 */
static inline TYPE* FUNC_NAME(front)(NAME *stk)
{
    return stk->data + stk->num_elem - 1;
}

/**
 * @brief Returns a const-pointer to the top element of the Stack.
 *
 * @attention For fast access, this method does not perform a check if the Stack
 * is empty. It is the user's responsibility to ensure the Stack is not empty.
 * 
 * @param stk Stack to query.
 * 
 * @return const TYPE* Const-pointer to the top element.
 */
static inline const TYPE* FUNC_NAME(front_cst)(const NAME *stk)
{
    return stk->data + stk->num_elem - 1;
}

/**
 * @brief Returns the number of elements on the Stack.
 * 
 * @param stk Stack to query.
 *
 * @return uint64 Number of elements. 
 */
static inline uint64 FUNC_NAME(size)(const NAME *stk)
{
    return stk->num_elem;
}

/**
 * @brief Returns the current capacity of the Stack.
 * 
 * @param stk Stack to query.
 * 
 * @return uint64 Current capacity.
 */
static inline uint64 FUNC_NAME(capacity)(const NAME *stk)
{
    return stk->cap;
}


#ifdef __cplusplus
} // extern "C"
#endif


#undef TYPE
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME
