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

#include "tundra/utils/memory/MemoryAlloc.hpp"
#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Str
{

namespace Internal
{

// Default memory alignment. 
constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 32;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for procedurally adding characters to a
 * null terminated array.
 * 
 * The String must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `free` method when the String is no longer required.
 * 
 * Internals are read-only.
 *
 * @tparam alignment Alignment in bytes to align the String's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<Tundra::uint8 alignment = Tundra::Str::Internal::DEFAULT_ALIGNMENT>
struct String 
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

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
template<Tundra::uint8 alignment>
inline void underlying_init(Tundra::Str::String<alignment> *str, 
    Tundra::uint64 init_capacity)
{
    str->chars = (char*)Tundra::alloc_aligned<alignment>(init_capacity);
    
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
template<Tundra::uint8 alignment>
inline void place_null_terminator(Tundra::Str::String<alignment> *str)
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
template<Tundra::uint8 alignment>
inline void check_and_handle_resize(Tundra::Str::String<alignment> *str)
{
    if(str->num_chars < str->capacity) { return; }

    // Get a new memory block that is twice the capacity of the current one.
    char* new_memory = (char*)Tundra::alloc_aligned<alignment>(
        str->capacity * 2);

    Tundra::copy_aligned_mem<alignment>(str->chars, new_memory, str->num_chars);

    Tundra::free_aligned(str->chars);
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
template<Tundra::uint8 alignment>
inline void underlying_shrink(Tundra::Str::String<alignment> *str, 
    Tundra::uint64 capacity)
{
    char* new_memory = (char*)Tundra::alloc_and_copy_aligned_mem<alignment>(
        str->chars, capacity, capacity + 1);

    Tundra::free_aligned(str->chars);
    str->chars = new_memory;
    str->num_chars = capacity;

    Tundra::Str::Internal::place_null_terminator(str);
}
    
} // namespace Internal


// Public ---------------------------------------------------------------------

/**
 * @brief Ensures the String has the capacity to store `extra_elements`,
 * resizing and reallocating characters if necessary.
 * 
 * @param str Pointer to the String 
 * @param extra_chars Number of extra characters.
 */
template<Tundra::uint8 alignment>
inline void reserve_for(Tundra::Str::String<alignment> *str, 
    Tundra::uint64 extra_chars)
{
    str->capacity = Tundra::reserve_aligned_mem<alignment>((void**)&str->chars,
        extra_chars, str->num_chars, str->capacity);
}

/**
 * @brief Initializes a String with default capacity. Allocates memory and
 * resets internal components.
 * 
 * @param str Pointer to the String.
 */
template<Tundra::uint8 alignment>
inline void init(Tundra::Str::String<alignment> *str)
{
    Tundra::Str::Internal::underlying_init(str, 
        Tundra::Str::Internal::DEFAULT_CAPACITY);
    Tundra::Str::Internal::place_null_terminator(str);
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
template<Tundra::uint8 alignment>
inline void init(Tundra::Str::String<alignment> *str, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (init_capacity == 0) *
        Tundra::Str::Internal::DEFAULT_CAPACITY;

    Tundra::Str::Internal::underlying_init(str, init_capacity);
    Tundra::Str::Internal::place_null_terminator(str);
}

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
template<Tundra::uint8 alignment>
inline void init(Tundra::Str::String<alignment> *str, const char* chars, 
    Tundra::uint64 num_chars)
{
    if(num_chars == 0)
    {
        Tundra::Str::init(str);
        return;
    }

    // Add 1 here to account for the null terminator as well as the chars.
    Tundra::alloc_and_reserve_aligned_mem<alignment>((void**)&str->chars, 
        &str->capacity, num_chars + 1);

    Tundra::copy_mem(chars, str->chars, num_chars);

    str->num_chars = num_chars;

    Tundra::Str::Internal::place_null_terminator(str);
}

/**
 * @brief Releases heap memory allocated for the String.
 *
 * After calling this method, the String should not be used unless 
 * reinitialized.
 *
 * It is safe to call this method on a String that has already been 
 * freed, or never 
 * 
 * @tparam alignment 
 * @param str 
 */
template<Tundra::uint8 alignment>
inline void free(Tundra::Str::String<alignment> *str)
{
    if(!str->chars) { return; } 

    Tundra::free_aligned(str->chars);
    str->chars = NULL;
}

/**
 * @brief Resets the String to an empty State.
 *
 * This does not modify, shrink, deallocate or zero out the underlying memory. 
 * Only the element count is reset to zero, so subsequent adds will overwrite 
 * previous data from the start of the Array.
 * 
 * @param str Pointer to the String. 
 */
template<typename T, Tundra::uint8 alignment>
inline void clear(Tundra::Str::String<alignment> *str)
{
    str->num_chars = 0;
}

/**
 * @brief Adds a single character to the String.
 * 
 * @param str Pointer to the String.
 * @param character Character to add.
 */
template<Tundra::uint8 alignment>
inline void add_char(Tundra::Str::String<alignment> *str, char character)
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
template<Tundra::uint8 alignment>
inline void add_chars(Tundra::Str::String<alignment> *str, const char* chars,
    Tundra::uint64 num_chars)
{
    // Add 1 to num_chars to account for null terminator.
    Tundra::reserve_aligned_mem<alignment>((void**)&str->chars, num_chars, 
        str->num_chars, str->capacity);

    // Subtract 1 here to start the copying at the null terminator position.
    Tundra::copy_mem(chars, str->chars + str->num_chars - 1, num_chars);

    // Subtract 1 here so that when null terminator is placed it is placed right
    // at the end.
    str->num_chars += num_chars - 1;

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
template<Tundra::uint8 alignment>
inline void resize(Tundra::Str::String<alignment> *str, Tundra::uint64 num_chars)
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
template<Tundra::uint8 alignment>
inline void shrink_to_capacity(Tundra::Str::String<alignment> *str, 
    Tundra::uint64 capacity)
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
template<Tundra::uint8 alignment>
inline void shrink_to_fit(Tundra::Str::String<alignment> *str)
{
    if(str->num_chars == str->capacity) { return; }

    // Subtract 1 here since the underlying shrink method assumes the shrink
    // capacity does not include the null terminator count in the capacity. 
    Tundra::Str::Internal::underlying_shrink(str, str->num_chars - 1);
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
template<Tundra::uint8 alignment>
inline bool erase(Tundra::Str::String<alignment> *str, Tundra::uint64 index)
{
    // Subtract 1 to not allowing erasure of the null terminator.
    if(index >= str->num_chars - 1) { return false; }

     // Copy the elements ahead of the index back one position.
    // memmove(str->chars + index, 
    //     str->chars + index + 1,
    //     (str->num_chars - index - 1));

    Tundra::copy_mem(str->chars + index + 1, str->chars + index, 
        str->num_chars - index - 1);

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
template<Tundra::uint8 alignment>
inline char* back(Tundra::Str::String<alignment> *str)
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
template<Tundra::uint8 alignment>
inline char* at_unchecked(Tundra::Str::String<alignment> *str, Tundra::uint64 index)
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
template<Tundra::uint8 alignment>
inline char* at(Tundra::Str::String<alignment> *str, Tundra::uint64 index)
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
template<Tundra::uint8 alignment>
inline const char* peek_unchecked(const Tundra::Str::String<alignment> *str, 
    Tundra::uint64 index)
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
template<Tundra::uint8 alignment>
inline const char* peek(const Tundra::Str::String<alignment> *str, Tundra::uint64 index)
{
    // Subtract 1 from num_chars to not allow indexing of the null terminator.
    return (index < str->num_chars - 1) ? str->chars + index : NULL;
}

/**
 * @brief Returns the memory alignment of the String.
 * 
 * @param str Pointer to the String. 
 *
 * @return [Tundra::uint8] Memory alignment of the String. 
 */
template<typename T, Tundra::uint8 alignment>
constexpr Tundra::uint8 get_alignment(
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
 * @return [Tundra::uint64] Number of characters in the String excluding the null
 *    terminator. 
 */
template<Tundra::uint8 alignment>
inline Tundra::uint64 size(const Tundra::Str::String<alignment> *str)
{
    return str->num_chars - 1;
}

/**
 * @brief Compares two String's content and returns true if they are equal.
 * 
 * @tparam alignment_first Alignment in bytes of the first String.
 * @tparam alignment_second Alignment in bytes of the second String.
 * @param first Pointer to the first String.
 * @param second Pointer to the second String.
 * @return bool True if the String's content are equal.
 */
template<Tundra::uint8 alignment_first, Tundra::uint8 alignment_second>
inline bool compare(const Tundra::Str::String<alignment_first> *first,
    const Tundra::Str::String<alignment_second> *second)
{
    if(first->num_chars != second->num_chars) { return false; }

    return Tundra::compare_mem((void*)first->chars, (void*)second->chars,
        first->num_chars);
}

template<Tundra::uint8 alignment>
inline Tundra::uint64 hash(const Tundra::Str::String<alignment> *str)
{
    // Uses the FNV-1a hash method.

    uint64 hash = 14695981039346656037ULL; // FNV offset basis
    const uint64 FNV_PRIME = 1099511628211ULL; 

    for(uint64 i = 0; i < str->num_chars - 1; ++i)
    {
        hash ^= str->chars[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

} // namespace Tundra::Str
