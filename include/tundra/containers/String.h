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
 * `num_extra_char` additional characters.
 * 
 * Assumes that the current number of characters plus `num_extra_char` exceeds the
 * current capacity.
 * 
 * @param str String to handle. 
 * @param num_extra_char Number of extra characters.
 */
void InTundra_Str_reserve_for(Tundra_String *str, uint64 num_extra_char);

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
 * @param str String to init, 
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
 * @param str String to init. 
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

/**
 * @brief Deep copies `src` to `dst`.
 *
 * If the Strings are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized String. 
 * 
 * @param src String to source from. 
 * @param dst String to deep copy to, can be uninitialized. 
 */
void Tundra_Str_copy(const Tundra_String *src, Tundra_String *dst); 

/**
 * @brief Transfers ownership of resources from `src` to `dst`. `src` is left in
 * an uninitialized state.
 *
 * If the Strings are of the same address, nothing is done.
 *
 * `dst` can be an uninitialized String.
 * 
 * @param src String to source from. 
 * @param dst String to transfer resources to, can be uninitialized.
 */
void Tundra_Str_move(Tundra_String *src, Tundra_String *dst);

/**
 * @brief Clears the String of all characters.
 *
 * Does not modify, shrink, deallocate or zero out underlying memory. Only the 
 * element count is reset to zero so subsequent adds will overwrite previous 
 * data from the start of the String.
 * 
 * @param str String to clear. 
 */
void Tundra_Str_clear(Tundra_String *str);

/**
 * @brief Adds a single character to the String.
 * 
 * @param str String to add to.
 * @param ch Character to add.
 */
void Tundra_Str_add(Tundra_String *str, char ch);

/**
 * @brief Adds a sequence of chars to the String.
 * 
 * `chars` does not need to be null terminated, if it is, ensure that `num_char`
 * does not include the terminator in its count.
 * 
 * @param str String to add to.
 * @param chars Array of chars to add.
 * @param num_char Number of chars in `chars`.
 */
void Tundra_Str_add_multiple(Tundra_String *str, const char *chars, 
    uint64 num_char);

/**
 * @brief Inserts a character at a position, shifting all elements ahead of 
 * it forward by one.
 *
 * A fatal is thrown if the index is out of range with the String unmodified.
 * 
 * @param str String to insert into.
 * @param ch Char to insert.
 * @param index Insert index.
 */
void Tundra_Str_insert(Tundra_String *str, char ch, uint64 index);

/**
 * @brief Resizes the String to contain `num_char' characters..
 * 
 * If `num_elem` is greater than the current capacity, the String is expanded to
 * hold the new number of elements. If `num_elem` is less than the current 
 * number of elements, excess elements are discarded.
 * 
 * If `num_char` is greater than the current number of characters, the new 
 * characters are uninitialized. The null terminator is moved to the end of the 
 * resized block.
 * 
 * @param arr String to resize.
 * @param num_char Number of characters to resize to.
 */
void Tundra_Str_resize(Tundra_String *str, uint64 num_char);

/**
 * @brief Expands the String to ensure it has the capacity to store 
 * `num_extra_char` additional characters.
 * 
 * @param str String to handle. 
 * @param num_extra_char Number of extra characters.
 */
void Tundra_Str_reserve(Tundra_String *str, uint64 num_extra_char);

/**
 * @brief Shrinks the String's allocated capacity to a specified capacity.
 *
 * If `new_cap` is greater than or equal to the current capacity, the String is
 * not modified. If `new_cap` is less than the current number of elements, 
 * excess elements are discarded and the String is resized to the value 
 * specified.
 * 
 * `capacity` should not account for the null terminator. Simply specify
 * the capacity of typical readable character capacity to shrink to.
 *
 * Memory is reallocated if the capacity is reduced. 
 *
 * @param str String to shrink. 
 * @param new_cap Capacity to shrink to.
 */
void Tundra_Str_shrink_to_new_cap(Tundra_String *str, uint64 capacity);

/**
 * @brief Shrinks the String's allocated capacity to match its current number of 
 * elements, including the null terminator.
 *
 * Memory is reallocated if capacity does not match current number of elements.
 * 
 * @param str String to shrink. 
 */
void Tundra_Str_shrink_to_fit(Tundra_String *str);

/**
 * @brief Removes the character at the specified index and shifts subsequent
 * characters back by one.
 *
 * A fatal is thrown if the index is out of range with the String unmodified.
 * 
 * @param str String to erase from.
 * @param index Index to erase.
 */
void Tundra_Str_erase(Tundra_String *str, uint64 index);

/**
 * @brief Returns a pointer to the first character in the String.
 * 
 * @attention For fast access, this method does not perform a size check on
 * the String. It is the user's responsibility to ensure the String is not
 * empty.
 * 
 * @param str String to query. 
 * 
 * @return char* Pointer to the first character in the String. 
 */
char* Tundra_Str_front(Tundra_String *str);

/**
 * @brief Returns a pointer to the first character in the String.
 * 
 * @attention For fast access, this method does not perform a size check on
 * the String. It is the user's responsibility to ensure the String is not
 * empty.
 * 
 * @param str String to query. 
 * 
 * @return const char* Pointer to the first character in the String. 
 */
const char* Tundra_Str_front_cst(const Tundra_String *str);

/**
 * @brief Returns a pointer to the last character in the String.
 * 
 * @attention For fast access, this method does not perform a size check on
 * the String. It is the user's responsibility to ensure the String is not
 * empty.
 * 
 * @param str String to query. 
 * 
 * @return char* Pointer to the last character in the String. 
 */
char* Tundra_Str_back(Tundra_String *str);

/**
 * @brief Returns a pointer to the last character in the String.
 * 
 * @attention For fast access, this method does not perform a size check on
 * the String. It is the user's responsibility to ensure the String is not
 * empty.
 * 
 * @param str String to query. 
 * 
 * @return const char* Pointer to the last character in the String. 
 */
const char* Tundra_Str_back_cst(const Tundra_String *str);

/**
 * @brief Returns a pointer to the character at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid.
 *  
 * @param str String to index into. 
 * @param index Index into the String.
 * 
 * @return char* Pointer to the character at `index`. 
 */
char* Tundra_Str_at_nocheck(Tundra_String *str, uint64 index);

/**
 * @brief Returns a const-pointer to the character at `index`.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid.
 *  
 * @param str String to index into. 
 * @param index Index into the String.
 * 
 * @return const char* Const-pointer to the character `index`. 
 */
const char* Tundra_Str_at_nocheck_cst(const Tundra_String *str, uint64 index);

/**
 * @brief Returns a pointer to the character at `index` with bounds checking.
 *
 * A fatal is thrown if the index is out of range with the String unmodified.
 * 
 * @param str String to index into.
 * @param index Index into the String.
 *
 * @return char* Pointer to the character at `index`.
 */
char* Tundra_Str_at(Tundra_String *str, uint64 index);

/**
 * @brief Returns a const-pointer to the character at `index` with bounds 
 * checking.
 *
 * A fatal is thrown if the index is out of range with the String unmodified.
 * 
 * @param str String to index into.
 * @param index Index into the String.
 *
 * @return const char* Const-pointer to the character at `index`.
 */
const char* Tundra_Str_at_cst(const Tundra_String *str, uint64 index);

/**
 * @brief Returns a const-pointer to the internal character array of the 
 * String.
 * 
 * @param str String to query.
 * 
 * @return const char* Const-pointer to the internal character array.
 */
const char* Tundra_Str_data_cst(const Tundra_String *str);

/**
 * @brief Returns the number of characters in the String, not including the null 
 * terminator.
 * 
 * @param str String to query.
 * 
 * @return uint64 Number of readable characters.
 */
uint64 Tundra_Str_size(const Tundra_String *str);

/**
 * @brief Returns the current capacity of the String, not counting the null
 * terminator.
 * 
 * @param str String to query.
 * 
 * @return uint64 Current capacity not counting null terminator.
 */
uint64 Tundra_Str_capacity(const Tundra_String *str);


#ifdef __cplusplus
} // extern "C" 
#endif

#endif
