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
 * Internals are read-only
 */
template<uint8_t alignment = 32>
struct String 
{
    char* chars;

    uint64_t num_chars;
    uint64_t capacity;
};


// Internal Methods ---------------------------------------------------------------

namespace Internal
{

constexpr uint64_t TUNDRA_STR_DEFAULT_CAPACITY = 2;
 
template<uint8_t alignment>
void underlying_init(Tundra::Str::String<alignment> *str, 
    uint64_t init_capacity)
{
    str->data = (char*)Tundra::aligned_alloc(alignment, init_capacity);

    str->num_chars = 0;
    str->capacity = init_capacity;
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
    init_capacity += 2 * (init_capacity == 0);

    Tundra::Str::Internal::underlying_init(str, init_capacity);
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

    Tundra::Str::Internal::underlying_init(str, num_chars);
}

} // namespace Tundra::Str
