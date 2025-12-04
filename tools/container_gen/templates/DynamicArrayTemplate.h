/**
 * @file DynamicArrayTemplate.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Template for a DynamicArray component with specified type. Used by a 
 * generator script to create type-specific DynamicArrays.
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing contiguous container for storing procedurally 
 * added elements.
 *
 * Must be initialized using either the `init`, `copy` or `move` methods before 
 * use. Must be freed on end of use using the `free` method.
 *
 * Internals are read-only.
 */
typedef struct NAME
{
    // Heap allocated array of elements.
    TYPE *data; 

    // Number of elements currently added to the Array.
    uint64 num_elem;

    // Current maximum number of elements that can be added to the Array before 
    // it needs to be resized.
    uint64 cap;

    // Copy function invoked when the `copy` method is called. 
    void (*copy_func)(const TYPE*, TYPE*, uint64);
    
    // Free function invoked when the `free` method is called.
    // void (*free_func)(TYPE*, uint64);
} NAME;


// Internal Methods ------------------------------------------------------------

void INT_FUNC_NAME(init)(NAME *arr, uint64 init_cap);

/**
 * @brief Default copy function that performs a memory copy of elements.
 * 
 * @param src Source elements to copy from.
 * @param dst Destination elements to copy to.
 * @param num_elem Number of elements to copy.
 */
void INT_FUNC_NAME(def_copy)(const TYPE *src, TYPE *dst, 
    uint64 num_elem);

// Public Methods --------------------------------------------------------------

/**
 * @brief Initializes an Array with default capacity. Allocates memory and 
 * sets internal components.
 *
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 * 
 * @param arr Array to init. 
*/
void FUNC_NAME(init)(NAME *arr);

/**
 * @brief Initialize an Array with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the Array is initialized with default capacity.
 *
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 * 
 * @param arr Array to init, 
 * @param init_cap Specified initial capacity.
 */
void FUNC_NAME(init_w_cap)(NAME *arr, uint64 init_cap);

/**
 * @brief Initializes an Array with initial elements. Allocates memory and sets
 * internal components.
 *
 * `elements` are copied into the Array. `num_elem` specifies the number of 
 * elements (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_elem` should be allocated for the Array. If 
 * this flag is false, the smallest power of 2 that can hold `num_elem` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 *
 * `elements` must not be memory inside the Array.
 * 
 * Only initialize an Array once. If an already initialized Array is called with
 * init, undefined behavior may occur. 
 *
 * @param arr Array to init. 
 * @param elements Array of elements to copy in.
 * @param num_elem Number of elements in `elements`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_elem`.
 */
void FUNC_NAME(init_w_elems)(NAME *arr, const TYPE *elements, 
    uint64 num_elem, bool strict_alloc);