/**
 * @file String.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for procedurally adding characters to a
 *        null terminated array.
 * @version 0.1
 * @date 07-20-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "tundra/utils/Memory.hpp"


namespace Tundra::Str
{

namespace Internal
{

// Default memory alignment. 
constexpr uint8_t DEFAULT_ALIGNMENT = 32;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for procedurally adding characters to a
 * null terminated array.
 * 
 * The String must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `deconstruct` method when the String is no longer required.
 * 
 * Internals are read-only.
 *
 * @tparam alignment Alignment in bytes to align the String's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<uint8_t alignment = Tundra::Str::Internal::DEFAULT_ALIGNMENT>
struct String 
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

    // Heap allocated array of characters.
    char* chars;

    // Number of characters in the String, including the null terminator.
    uint64_t num_chars;

    // Current maximum number of characters that the String can store before it
    // needs to be resized.
    uint64_t capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

// Default capacity in characters of a String.
constexpr uint64_t DEFAULT_CAPACITY = 4;
 

/**
 * @brief Underlying initialization method. Allocates initial memory  for 
 * `init_capacity` characters and sets internal values.
 * 
 * @param str Pointer to the String. 
 * @param init_capacity Initial capacity in characters to allocate.
 */
template<uint8_t alignment>
void underlying_init(Tundra::Str::String<alignment> *str, 
    uint64_t init_capacity)
{
    str->chars = (char*)Tundra::aligned_alloc<alignment>(init_capacity);
    
    str->num_chars = 0;
    str->capacity = init_capacity;
}

/**
 * @brief Places a null terminator at the end of the characters in the String.
 *
 * Internal method, so we assume that there is enough space for the terminator.
 * 
 * @param str Pointer to the String. 
 */
template<uint8_t alignment>
void place_null_terminator(Tundra::Str::String<alignment> *str)
{
    str->chars[str->num_chars] = '\0';
    ++str->num_chars;
}
    
/**
* @brief Checks if the String has filled its allocated capacity, resizing if it
* has.
* 
* @param str Pointer to the String.
*/
template<uint8_t alignment>
void check_and_handle_resize(Tundra::Str::String<alignment> *str)
{
    if(str->num_chars < str->capacity) { return; }

    // Get a new memory block that is twice the capacity of the current one.
    char* new_memory = (char*)Tundra::aligned_alloc(alignment, 
        str->capacity * 2);

    Tundra::copy_aligned_mem<alignment>(str->chars, new_memory, str->num_chars);

    Tundra::aligned_free(str->chars);
    str->chars = new_memory;
    str->capacity *= 2;
}

/**
 * @brief Underlying shrink method, shrinks the String's capacity to match the 
 * specified new capacity.
 *  
 * `capacity` does not include the space for the null terminator.
 *
 * @param arr Pointer to the String.
 * @param capacity Capacity to shrink to.
 */
template<uint8_t alignment>
void underlying_shrink(Tundra::Str::String<alignment> *str, 
    uint64_t capacity)
{
    char* new_memory = (char*)Tundra::alloc_and_copy_aligned_mem<alignment>(
        str->chars, capacity, capacity + 1);

    Tundra::aligned_free(str->chars);
    str->chars = new_memory;
    str->num_chars = capacity;

    Tundra::Str::Internal::place_null_terminator(str);
}
    
} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Initializes a String with default capacity. Allocates memory and
 * resets internal components.
 * 
 * @param str Pointer to the String.
 */
template<uint8_t alignment>
void init(Tundra::Str::String<alignment> *str)
{
    Tundra::Str::Internal::underlying_init(str, 
        Tundra::Str::Internal::DEFAULT_CAPACITY);
}

/**
 * @brief Initializes a String with a specified capacity. Allocates memory and 
 * resets internal components.
 * 
 * If `init_capacity` is 0, it is set to the default capacity.
 * 
 * @tparam alignment 
 * @param str 
 * @param init_capacity 
 */
template<uint8_t alignment>
void init(Tundra::Str::String<alignment> *str, uint64_t init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) *
        Tundra::Str::Internal::DEFAULT_CAPACITY;

    Tundra::Str::Internal::underlying_init(str, init_capacity);
    Tundra::Str::Internal::place_null_terminator(str);
}

template<uint8_t alignment>
void init(Tundra::Str::String<alignment> *str, const char* chars, 
    uint64_t num_chars)
{
    if(num_chars == 0)
    {
        Tundra::Str::init(str);
        return;
    }

    // Add 1 here to account for the null terminator as well as the chars.
    Tundra::Str::Internal::underlying_init(str, num_chars + 1);

    Tundra::copy_mem(chars, str->chars, num_chars);

    str->num_chars += num_chars;

    Tundra::Str::Internal::place_null_terminator(str);
}

/**
 * @brief Releases heap memory allocated for the String.
 *
 * After calling this method, the String should not be used unless 
 * reinitialized.
 *
 * It is safe to call this method on a String that has already been 
 * deconstructed, or never 
 * 
 * @tparam alignment 
 * @param str 
 */
template<uint8_t alignment>
void deconstruct(Tundra::Str::String<alignment> *str)
{
    if(!str->chars) { return; } 

    Tundra::aligned_free(str->chars);
}

/**
 * @brief Ensures the String has the capacity to store `extra_elements`,
 * resizing and reallocating characters if necessary.
 * 
 * @param str Pointer to the String 
 * @param extra_chars Number of extra characters.
 */
template<uint8_t alignment>
void reserve_for(Tundra::Str::String<alignment> *str, 
    uint64_t extra_chars)
{
    str->capacity = Tundra::reserve_aligned_mem<alignment>((void**)&str->chars,
        extra_chars, str->num_chars, str->capacity);
}

/**
 * @brief Adds a single character to the String.
 * 
 * @param str Pointer to the String.
 * @param character Character to add.
 */
template<uint8_t alignment>
void add_char(Tundra::Str::String<alignment> *str, char character)
{
    Tundra::Str::Internal::check_and_handle_resize(str);

    str->chars[str->num_chars - 1] = character;
    Tundra::Str::Internal::place_null_terminator(str);
}

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
template<uint8_t alignment>
void add_chars(Tundra::Str::String<alignment> *str, const char* chars,
    uint64_t num_chars)
{
    // Add 1 to num_chars to account for null terminator.
    Tundra::reserve_aligned_mem<alignment>((void**)&str->chars, num_chars, 
        str->num_chars + 1, str->capacity);

    Tundra::copy_mem(chars, str->chars + str->num_chars, num_chars);

    Tundra::Str::Internal::place_null_terminator(str);
}

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
template<uint8_t alignment>
void resize(Tundra::Str::String<alignment> *str, uint64_t num_chars)
{
    // Account for the null terminator.
    num_chars += 1;

    if(num_chars <= str->num_chars) { return; }

    if(num_chars > str->num_chars)
    {
        Tundra::Str::reserve_for(str, num_chars - str->num_chars);
    }

    // Deduct 1 to prepare the place_null_terminator method to put the 
    // terminator in the correct spot, which is one less than the capacity
    // we just resized.
    str->num_chars = num_chars - 1;
    Tundra::Str::Internal::place_null_terminator(str);
}


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
template<uint8_t alignment>
void shrink_to_capacity(Tundra::Str::String<alignment> *str, 
    uint64_t capacity)
{
    // +1 to account for null terminator.
    if(capacity + 1 >= str->capacity) { return; }

    Tundra::Str::Internal::underlying_shrink(str, capacity);
}

/**
 * @brief Reduces the String's allocated capacity to match its current number of 
 * characters (including the null terminator).
 *
 * Memory is reallocated if capacity does not match current number of 
 * characters, counting the null terminator.
 *
 * @param arr Pointer to the String.
 */
template<uint8_t alignment>
void shrink_to_fit(Tundra::Str::String<alignment> *str)
{
    if(str->num_chars == str->capacity) { return; }

    // Subtract 1 here since the underlying shrink method assumes the shrink
    // capacity does not include the null terminator count in the capacity. 
    Tundra::Str::Internal::underlying_shrink(str, str->num_elements - 1);
}

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
template<uint8_t alignment>
bool erase(Tundra::Str::String<alignment> *str, uint64_t index)
{
    // Subtract 1 to not allowing erasure of the null terminator.
    if(index >= str->num_chars - 1) { return false; }

     // Copy the elements ahead of the index back one position.
    memmove(str->chars + index, 
        str->data + index + 1,
        (str->num_chars - index - 1));

    --str->num_chars;
    return true;
} 

/**
 * @brief Returns a pointer to the last character in the String.
 *
 * Assumes the String is not empty. Calling it with an empty String will return
 * the null terminator.
 * 
 * @param str Pointer to the String.
 * 
 * @return char* Pointer to the last character in the String. 
 */
template<uint8_t alignment>
char* back(Tundra::Str::String<alignment> *str)
{
    return str->chars + str->num_chars - 1;
}

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
template<uint8_t alignment>
char* at_unchecked(Tundra::Str::String<alignment> *str, uint64_t index)
{
    return str->chars + index;
}

/**
 * @brief Returns a pointer to the value at an index with bounds checking.
 *
 * Performs bounds checking on `index`, returning NULL if it's invalid.
 * 
 * @param str Pointer to the String.
 * @param index Index into the String.
 *
 * @return char* Pointer to the item at the index, or NULL if index is invalid.
 */
template<uint8_t alignment>
char* at(Tundra::Str::String<alignment> *str, uint64_t index)
{
    // Subtract 1 from num_chars to not allow indexing of the null terminator.
    return (index < str->num_chars - 1) ? str->chars + index : NULL;
}

/**
 * @brief Returns a read-only pointer to the value at an index.
 *
 * @attention For fast access, this method does not perform a bounds check on 
 * `index`. It is the user's responsibilty to ensure index is valid.
 * 
 * @param str Pointer to the String.
 * @param index Index into the String.
 *
 * @return [const char*] Read-only pointer to the character at the index. 
 */
template<uint8_t alignment>
const char* peek_unchecked(const Tundra::Str::String<alignment> *str, 
    uint64_t index)
{
    return str->chars + index;
}

/**
 * @brief Returns a read-only pointer to the value at an index with bounds 
 * checking.
 * 
 * @param str Pointer to the String.
 * @param index Index into the String.
 *
 * @return [const char*] Read-only pointer to the character at the index.
 */
template<uint8_t alignment>
const char* peek(const Tundra::Str::String<alignment> *str, uint64_t index)
{
    // Subtract 1 from num_chars to not allow indexing of the null terminator.
    return (index < str->num_chars - 1) ? str->chars + index : NULL;
}

/**
 * @brief Returns the memory alignment of the String.
 * 
 * @param str Pointer to the String. 
 *
 * @return [uint8_t] Memory alignment of the String. 
 */
template<typename T, uint8_t alignment>
constexpr uint8_t get_alignment(
    const Tundra::Str::String<alignment> *str) // NOLINT
{
    return alignment;
}

/**
 * @brief Returns the number of characters in the String, not including the 
 * null terminator.
 * 
 * @param str Pointer to the String.
 * 
 * @return [uint64_t] Number of characters in the String excluding the null
 *    terminator. 
 */
template<uint8_t alignment>
uint64_t size(Tundra::Str::String<alignment> *str)
{
    return str->num_chars - 1;
}



} // namespace Tundra::Str
