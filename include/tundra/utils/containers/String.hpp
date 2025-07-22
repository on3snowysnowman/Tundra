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

constexpr uint8_t DEFAULT_ALIGNMENT = 32;

} // namespace Internal


// Containers ------------------------------------------------------------------

/**
 * @brief Automatic resizing container for procedurally adding characters to a
 * null terminated array.
 * 
 * The string must be initialized using the `init` method(s) before it is used.
 * 
 * Some memory for this component is heap allocated and must be manually freed 
 * using the `deconstruct` method when the string is no longer required.
 * 
 * Internals are read-only.
 *
 * @tparam alignment Alignment in bytes to align the String's heap memory
 *    (allows SIMD instruction use for fast reallocation).
 */
template<uint8_t alignment = Tundra::Str::Internal::DEFAULT_ALIGNMENT>
struct String 
{
    char* chars;

    uint64_t num_chars;
    uint64_t capacity;
};


// Internal --------------------------------------------------------------------

namespace Internal
{

constexpr uint64_t TUNDRA_STR_DEFAULT_CAPACITY = 2;
 

/**
 * @brief 
 * 
 * @tparam alignment 
 * @param str 
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

template<uint8_t alignment>
void underlying_init(Tundra::Str::String<alignment> *str, 
    uint64_t init_capacity)
{
    str->chars = (char*)Tundra::aligned_alloc(alignment, init_capacity);

    str->num_chars = 0;
    str->capacity = init_capacity;
}

template<uint8_t alignment>
void place_null_terminator(Tundra::Str::String<alignment> *str)
{
    str->chars[str->num_chars] = '\0';
    ++str->num_chars;
}


} // namespace Internal


// Public ---------------------------------------------------------------------

template<uint8_t alignment>
void init(Tundra::Str::String<alignment> *str)
{
    Tundra::Str::Internal::underlying_init(str, 
        Tundra::Str::Internal::TUNDRA_STR_DEFAULT_CAPACITY);
}

template<uint8_t alignment>
void init(Tundra::Str::String<alignment> *str, uint64_t init_capacity)
{
    init_capacity += (init_capacity == 0) *
        (Tundra::Str::Internal::TUNDRA_STR_DEFAULT_CAPACITY);

    Tundra::Str::Internal::underlying_init(str, init_capacity);

    Tundra::Str::Internal::place_null_terminator<alignment>(str);
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

    Tundra::Str::Internal::place_null_terminator<alignment>(str);
}

/**
 * @brief 
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
 * @brief Adds a single character to the String.
 * 
 * @param str Pointer to the String.
 * @param character Character to add.
 */
template<uint8_t alignment>
void add_char(Tundra::Str::String<alignment> *str, char character)
{
    Tundra::Str::Internal::check_and_handle_resize<alignment>(str);

    str->chars[str->num_chars - 1] = character;
    Tundra::Str::Internal::place_null_terminator<alignment>(str);
}

/**
 * @brief Adds a sequence of chars to the String. 
 * 
 * The number of characters in `chars` must be specified.
 * 
 * It is irrelevant if `chars` is null terminated or not, just ensure that if it
 * is, the terminator character is NOT included in final count of `size_chars`.
 * 
 * @param string Pointer to the String.
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

    Tundra::Str::Internal::place_null_terminator<alignment>(str);
}

} // namespace Tundra::Str
