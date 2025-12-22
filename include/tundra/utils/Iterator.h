/**
 * @file Iterator.h
 * @author your name (you@domain.com)
 * @brief Iterator functions for handling operations on container iterators. 
 *    Provides single functions for each iterator operation that can accept any
 *    Tundra Container iterator.
 * @version 0.1
 * @date 2025-12-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tundra/utils/CoreDef.h"

/**
 * @brief Stores container type specific function pointers for an iterator from 
 * a Tundra container.
 * 
 * This iterator data is placed at the beginning of each Tundra iterator 
 * struct, so a Tundra iterator can be casted to this struct to access its 
 * function pointers, allowing for handling of ambiguous iterator types.
 */
typedef struct InTundra_IterData
{
    void (*next)(void*);
    void (*prev)(void*);
    void* (*deref)(const void*);
    bool (*cmp)(const void*, const void*);
} InTundra_IterData;

/**
 * @brief Calls the `next` function of the passed iterator.
 * 
 * `iter` must be an iterator from a Tundra container, for example: 
 * DynamicArrayTYPE_Iter, LinkedListTYPE_Iter, etc.
 * 
 * @param iter Tundra container iterator.
 */
static inline void Tundra_Iter_next(void *iter)
{
    InTundra_IterData *iter_cast = (InTundra_IterData*)iter;

    iter_cast->next(iter);
}

/**
 * @brief Calls the `prev` function of the passed iterator.
 * 
 * `iter` must be an iterator from a Tundra container, for example: 
 * DynamicArrayTYPE_Iter, LinkedListTYPE_Iter, etc.
 * 
 * @param iter Tundra container iterator.
 */
static inline void Tundra_Iter_prev(void *iter)
{
    InTundra_IterData *iter_cast = (InTundra_IterData*)iter;

    iter_cast->prev(iter);
}

/**
 * @brief Calls the `deref` function of the passed iterator, and returns its 
 * return value.
 * 
 * `iter` must be an iterator from a Tundra container, for example: 
 * DynamicArrayTYPE_Iter, LinkedListTYPE_Iter, etc.
 * 
 * @param iter Tundra container iterator.
 */
static inline void* Tundra_Iter_deref(const void *iter)
{
    InTundra_IterData *iter_cast = (InTundra_IterData*)iter;

    return iter_cast->deref(iter);
}

/**
 * @brief Calls the `cmp` function of the passed iterator, and returns it 
 * return value.
 * 
 * Both `first_iter` and `second_iter` must be iterators from Tundra containers, 
 * for example: DynamicArrayTYPE_Iter, LinkedListTYPE_Iter, etc.
 * 
 * @param iter Tundra container iterator.
 */
static inline bool Tundra_Iter_compare(const void *first_iter, 
    const void *second_iter)
{
    const InTundra_IterData *first_cast = (const InTundra_IterData*)first_iter;
    return first_cast->cmp(first_iter, second_iter);
}
