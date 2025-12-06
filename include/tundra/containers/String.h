/**
 * @file String.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for procedurally adding characters to a
 *    null terminated array.
 * @date 2025-12-06
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_STRING_H
#define TUNDRA_STRING_H

#include "tundra/utils/CoreDef.h"

#define TUNDRA_STR_DEF_CAP 4


#ifdef __cplusplus
extern "C" {
#endif

// Containers ------------------------------------------------------------------

typedef struct Tundra_String
{
    // Heap allocated array of characters. 
    char* chars;

    // Number of characters in the String, including the null terminator.
    uint64 num_char;

    // Current maximum number of characters that the String can store before it
    // needs to be resized.
    uint64 cap;
} Tundra_String;


// Internal Methods ------------------------------------------------------------

/**
 * @brief Internal init method called by the public init methods. Allocates 
 * initial memory for `init_cap` chars and sets container components.
 * 
 * @param str String to initialize. 
 * @param init_cap Initial capacity in chars.
 */
void InTundra_Str_init(Tundra_String *str, uint64 init_cap);

/**
 * @brief Places a null terminator at the end of the characters in the String.
 *
 * Internal method, so we assume that there is enough space for the terminator.
 * 
 * @param str String to place in.
 */
void InTundra_Str_place_null_t(Tundra_String *str);

/**
 * @brief Checks if a String has filled its allocated capacity, expanding and
 * reallocating if it has.
 * 
 * @param str String to handle.
 */
void InTundra_Str_check_handle_exp(Tundra_String *str);

/**
 * @brief Expands the String to ensure it has the capacity to store 
 * `extra_chars` additional characters.
 * 
 * Assumes that the current number of characters plus `extra_chars` exceeds the
 * current capacity.
 * 
 * @param str String to handle. 
 * @param extra_chars Number of extra characters.
 */
void InTundra_Str_reserve_for(Tundra_String *str, uint64 extra_chars);

/**
 * @brief Internal shrink method, reallocates the String to a capacity of `cap`.
 * 
 * Does not place a null terminator after shrinking, caller must ensure that the
 * String is properly null terminated after this call.
 * 
 * @param str String to shrink. 
 * @param cap Capacity to shrink to.
 */
void InTundra_Str_shrink(Tundra_String *str, uint64 cap);


// Public Methods -------------------------------------------------------------

/**
 * @brief Initializes a String with default capacity. Allocates memory and 
 * sets internal components.
 *
 * Only initialize a String once. If an already initialized String is called 
 * with init, undefined behavior may occur. 
 * 
 * @param str String to init. 
*/
void Tundra_Str_init(Tundra_String *str);

/**
 * @brief Initialize a String with set capacity. Allocates memory and sets 
 * internal components.
 *
 * If `init_cap` is 0, the String is initialized with default capacity.
 *
 * Only initialize a String once. If an already initialized String is called 
 * with init, undefined behavior may occur. 
 * 
 * @param arr String to init, 
 * @param init_cap Specified initial capacity.
 */
void Tundra_Str_init_w_cap(Tundra_String *str, uint64 init_cap);

/**
 * @brief Initializes a String with initial chars. Allocates memory and sets
 * internal components.
 *
 * `chars` does not need to be null terminated, if it is, ensure that `num_char`
 * does not include the terminator in its count.
 * 
 * `chars` are copied into the String. `num_char` specifies the number of 
 * chars (not bytes) to copy in. `strict_alloc` is a flag to specify if 
 * exactly enough memory for `num_char` should be allocated for the String. If 
 * this flag is false, the smallest power of 2 that can hold `num_char` will 
 * be allocated to optimize against immediate reallocation on the next add 
 * request.
 * 
 * Only initialize a String once. If an already initialized String is called with
 * init, undefined behavior may occur. 
 *
 * @param arr String to init. 
 * @param chars String of chars to copy in.
 * @param num_char Number of chars in `chars`.
 * @param strict_alloc Whether to allocate only enough bytes for `num_char`.
 */
void Tundra_Str_init_w_chars(Tundra_String *str, const char* chars, 
    uint64 num_char, bool strict_alloc);

/**
 * @brief Frees memory allocated for a String.
 * 
 * After calling this method, the String must not be used unless reinitialized.
 * 
 * It is safe to call this method with a String that has already been freed, or
 * never initialized.
 * 
 * @param str String to free.
 */
void Tundra_Str_free(Tundra_String *str);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif