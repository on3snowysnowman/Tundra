/**
 * @file String.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for procedurally adding characters to a
 *        null terminated array.
 * @version 0.1
 * @date 07-20-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Str
{

// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for procedurally adding characters to a
 * null terminated array.
 * 
 * The String must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `Str::free` method when the String is no longer required.
 * 
 * Internals are read-only.
 */
struct String 
{
    // Heap allocated array of characters. 
    char* chars;

    // Number of characters in the String, including the null terminator.
    Tundra::uint64 num_chars;

    // Current maximum number of characters that the String can store before it
    // needs to be resized.
    Tundra::uint64 capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Default capacity in characters of a String.
constexpr Tundra::uint64 DEFAULT_CAPACITY = 4;
 

/**
 * @brief Underlying initialization method. Allocates initial memory  for 
 * `init_capacity` characters and sets internal values.
 * 
 * @param str Pointer to the String. 
 * @param init_capacity Initial capacity in characters to allocate.
 */
bool underlying_init(Tundra::Str::String &str, Tundra::uint64 init_capacity);

/**
 * @brief Places a null terminator at the end of the characters in the String.
 *
 * Internal method, so we assume that there is enough space for the terminator.
 * 
 * @param str Pointer to the String. 
 */
void place_null_terminator(Tundra::Str::String &str);
    
/**
* @brief Checks if the String has filled its allocated capacity, resizing if it
* has.
* 
* @param str Pointer to the String.
*/
bool check_and_handle_resize(Tundra::Str::String &str);

/**
 * @brief Underlying shrink method, shrinks the String's capacity to match the 
 * specified new capacity.
 *  
 * `capacity` does not include the space for the null terminator.
 *
 * @param arr Pointer to the String.
 * @param capacity Capacity to shrink to.
 */
bool underlying_shrink(Tundra::Str::String &str, Tundra::uint64 capacity);
    
} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Ensures the String has the capacity to store `extra_elements`,
 * resizing and reallocating characters if necessary.
 * 
 * @param str Pointer to the String 
 * @param extra_chars Number of extra characters.
 */
bool reserve_for(Tundra::Str::String &str, Tundra::uint64 extra_chars);

/**
 * @brief Initializes a String with default capacity. Allocates memory and
 * resets internal components.
 * 
 * @param str Pointer to the String.
 */
bool init(Tundra::Str::String &str);

/**
 * @brief Initializes a String with a specified capacity. Allocates memory and 
 * resets internal components.
 * 
 * If `init_capacity` is 0, it is set to the default capacity.
 * 
 * @param str 
 * @param init_capacity 
 */
bool init(Tundra::Str::String &str, Tundra::uint64 init_capacity);

/**
 * @brief Initializes a String with a set of characters. Allocates at least 
 * enough memory for `num_chars` and resets internal components.
 *
 * `chars` does not need to be null terminated. Ensure that the null terminator 
 * is not included in the count for `num_chars`.
 *
 * @param str Pointer to the String.
 * @param chars Pointer to the array of characters.
 * @param num_chars Number of chars to copy in, excluding the null terminator.
 */
bool init(Tundra::Str::String &str, const char* chars, 
    Tundra::uint64 num_chars);

/**
 * @brief Releases heap memory allocated for the String.
 *
 * After calling this method, the String should not be used unless 
 * reinitialized.
 * 
 * @param str 
 */
void free(Tundra::Str::String &str);

/**
 * @brief Copies the contents of `src` into `dst`, performing a deep copy.
 *
 * If `dst`'s memory is too small/large, it is automatically freed and resized,
 * so the user should not call free on `dst` before this method.
 *
 * @param dst Reference to the destination String.
 * @param src Read-only reference to the source String.
 */
bool copy(Tundra::Str::String &dst, const Tundra::Str::String &src);

/**
 * @brief Moves the contents of `src` into `dst`, transferring ownership of 
 * resources.
 * 
 * Memory for `dst` is automatically freed by this method. 
 *
 * After this operation, `src` is left in a null state, and should not be used
 * unless reinitialized. 
 *
 * @param dst Reference to the destination String.
 * @param src Rvalue reference to the source String.
 */
void move(Tundra::Str::String &dst, Tundra::Str::String &&src);

/**
 * @brief Resets the String to an empty State.
 *
 * This does not modify, shrink, deallocate or zero out the underlying memory. 
 * Only the element count is reset to zero, so subsequent adds will overwrite 
 * previous data from the start of the Array.
 * 
 * @param str Pointer to the String. 
 */
void clear(Tundra::Str::String &str);

/**
 * @brief Adds a single character to the String.
 * 
 * @param str Pointer to the String.
 * @param character Character to add.
 */
bool add_char(Tundra::Str::String &str, char character);

/**
 * @brief Adds a sequence of chars to the String. 
 * 
 * The number of characters in `chars` must be specified.
 * 
 * It is irrelevant if `chars` is null terminated or not, just ensure that if it
 * is, the terminator character is NOT included in final count of `size_chars`.
 * 
 * @param str Pointer to the String.
 * @param chars Pointer to the array of characters.
 * @param size_chars Number of characters in `chars`.
 */
bool add_chars(Tundra::Str::String &str, const char* chars,
    Tundra::uint64 num_chars);

/**
 * @brief Expands the String to hold at least `num_chars` indexable and 
 * modifiable elements.
 *
 * If `num_chars` is less than or equal to the current number of chars, the 
 * String remains unchanged. Otherwise, the String is resized to allow indexed
 * access up to `num_chars - 1`. If the internal capacity is insufficient, it is
 * increased and memory is reallocated as needed.
 *
 * Newly allocated elements are left uninitialized. The null terminator is moved
 * to the end of the resized block.
 *
 * This function does not shrink the String. To reduce the size, use one of the
 * shrink methods. 
 * 
 * @param str Pointer to the String.
 * @param num_chars Desired total numer of characters.
 */
bool resize(Tundra::Str::String &str, Tundra::uint64 num_chars);

/**
 * @brief Shrinks the String's allocated capacity to the specified value.
 * 
 * If `capacity` is greater than or equal to the current capacity, no changes 
 * are made. If `capacity` is less than the current number of elements, excess 
 * elements are discarded and the String is resized accordingly.
 * 
 * Memory is reallocated if the capacity is reduced.
 *
 * `capacity` should not account for the null terminator. Simply specify
 * the capacity of typical readable character capacity to shrink to, and 
 * `capacity` will be automatically incremented by 1 to account for null 
 * terminator placement.
 * 
 * @param str Pointer to the String.
 * @param capacity New capacity to shrink to.
 */
bool shrink_to_capacity(Tundra::Str::String &str, Tundra::uint64 capacity);

/**
 * @brief Reduces the String's allocated capacity to match its current number of 
 * characters (including the null terminator).
 *
 * Memory is reallocated if capacity does not match current number of 
 * characters, counting the null terminator.
 *
 * @param arr Pointer to the String.
 */
bool shrink_to_fit(Tundra::Str::String &str);

/**
 * @brief Removes the character at the specified index and shifts subsequent 
 * characters back by one position, returns true if successful.
 *
 * Performs bounds checking on the index.
 *
 * @param str Pointer to the String.
 * @param index Index of the element to remove.
 * 
 * @return True if erasure was successful, false otherwise.
 */
bool erase(Tundra::Str::String &str, Tundra::uint64 index);

/**
 * @brief Returns a pointer to the last character in the String.
 *
 * @attention For fast access, this method does not perform a size check on 
 * the String. It is the user's responsibility to ensure the String is not 
 * empty.
 * 
 * @param str Pointer to the String.
 * 
 * @return char* Pointer to the last character in the String. 
 */
char& back(Tundra::Str::String &str);

/**
 * @brief Returns a pointer to the value at an index.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibility to ensure the index is valid.
 *  
 * @param str Pointer to the String. 
 * @param index Index into the String.
 * 
 * @return char* Pointer to the character at the index. 
 */
char& at_unchecked(Tundra::Str::String &str, Tundra::uint64 index);

/**
 * @brief Returns a reference to the value at an index with bounds checking.
 *
 * Performs bounds checking on `index`, returning NULL if it's invalid.
 * 
 * @param str Pointer to the String.
 * @param index Index into the String.
 *
 * @return char* Pointer to the item at the index, or NULL if index is invalid.
 */
char& at(Tundra::Str::String &str, Tundra::uint64 index);

/**
 * @brief Returns a const reference to the value at an index with bounds 
 * checking.
 *
 * Performs bounds checking on `index`, returning NULL if it's invalid.
 * 
 * @param str Pointer to the String.
 * @param index Index into the String.
 *
 * @return char* Pointer to the item at the index, or NULL if index is invalid.
 */
const char& at(const Tundra::Str::String &str, Tundra::uint64 index);

// /**
//  * @brief Returns a read-only pointer to the value at an index.
//  *
//  * @attention For fast access, this method does not perform a bounds check on 
//  * `index`. It is the user's responsibilty to ensure index is valid.
//  * 
//  * @param str Pointer to the String.
//  * @param index Index into the String.
//  *
//  * @return const char* Read-only pointer to the character at the index. 
//  */
// const char& peek_unchecked(const Tundra::Str::String &str, 
//     Tundra::uint64 index);

// /**
//  * @brief Returns a read-only pointer to the value at an index with bounds 
//  * checking.
//  * 
//  * @param str Pointer to the String.
//  * @param index Index into the String.
//  *
//  * @return const char* Read-only pointer to the character at the index.
//  */
// const char& peek(const Tundra::Str::String &str, Tundra::uint64 index);

/**
 * @brief Returns the number of characters in the String, not including the 
 * null terminator.
 * 
 * @param str Pointer to the String.
 * 
 * @return Tundra::uint64 Number of characters in the String excluding the null
 *    terminator. 
 */
Tundra::uint64 size(const Tundra::Str::String &str);

/**
 * @brief Compares two String's content and returns true if they are equal.
 * 
 * @tparam alignment_first Alignment in bytes of the first String.
 * @tparam alignment_second Alignment in bytes of the second String.
 * @param first Pointer to the first String.
 * @param second Pointer to the second String.
 * @return bool True if the String's content are equal.
 */
bool compare(const Tundra::Str::String *first,
    const Tundra::Str::String *second);

Tundra::uint64 hash(const Tundra::Str::String &str);

/**
 * @brief Returns the underlying null terminated array of the String.
 * 
 * @param str Read-only pointer to the String.
 *
 * @return const char* Read-only pointer to the underlying char array. 
 */
const char* data(const Tundra::Str::String &str);

} // namespace Tundra::Str
