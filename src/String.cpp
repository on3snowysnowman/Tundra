/**
 * @file String.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 10-02-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/utils/containers/String.hpp"

#include <cstdlib>

#include "tundra/utils/memory/MemoryAlloc.hpp"


// Internal --------------------------------------------------------------------

void Tundra::Str::Internal::underlying_init(Tundra::Str::String *str, 
    Tundra::uint64 init_capacity)
{
    str->chars = (char*)malloc(init_capacity);
    
    str->num_chars = 0;
    str->capacity = init_capacity;
}

void Tundra::Str::Internal::place_null_terminator(Tundra::Str::String *str)
{
    str->chars[str->num_chars] = '\0';
    ++str->num_chars;
}

void Tundra::Str::Internal::check_and_handle_resize(Tundra::Str::String *str)
{
    if(str->num_chars < str->capacity) { return; }

    // Get a new memory block that is twice the capacity of the current one.
    char *new_memory = (char*)malloc(
        str->capacity * 2);

    Tundra::copy_mem(str->chars, new_memory, str->num_chars);

    ::free((void*)str->chars);
    // Tundra::free_aligned(str->chars);
    str->chars = new_memory;
    str->capacity *= 2;
}

void Tundra::Str::Internal::underlying_shrink(Tundra::Str::String *str, 
    Tundra::uint64 capacity)
{
    char *new_memory = (char*)Tundra::alloc_and_copy_mem((void*)str->chars, 
        capacity, capacity + 1);

    Tundra::free_aligned(str->chars);
    str->chars = new_memory;
    str->num_chars = capacity;

    Tundra::Str::Internal::place_null_terminator(str);
}


// Public ----------------------------------------------------------------------

void Tundra::Str::reserve_for(Tundra::Str::String *str, 
    Tundra::uint64 extra_chars)
{
    str->capacity = Tundra::reserve_mem((void**)&str->chars,
        extra_chars, str->num_chars, str->capacity);
}

void Tundra::Str::init(Tundra::Str::String *str)
{
    Tundra::Str::Internal::underlying_init(str, 
        Tundra::Str::Internal::DEFAULT_CAPACITY);
    Tundra::Str::Internal::place_null_terminator(str);
}

void Tundra::Str::init(Tundra::Str::String *str, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (Tundra::uint64)(init_capacity == 0) *
        Tundra::Str::Internal::DEFAULT_CAPACITY;

    Tundra::Str::Internal::underlying_init(str, init_capacity);
    Tundra::Str::Internal::place_null_terminator(str);
}

void Tundra::Str::init(Tundra::Str::String *str, const char* chars, Tundra::uint64 num_chars)
{
    if(num_chars == 0)
    {
        Tundra::Str::init(str);
        return;
    }

    // Add 1 here to account for the null terminator as well as the chars.
    Tundra::alloc_and_reserve_mem((void**)&str->chars, &str->capacity, 
        num_chars + 1);

    Tundra::copy_mem(chars, str->chars, num_chars);

    str->num_chars = num_chars;

    Tundra::Str::Internal::place_null_terminator(str);
}

void Tundra::Str::free(Tundra::Str::String *str)
{
    if(!(bool)str->chars) { return; } 

    Tundra::free_aligned(str->chars);
    str->chars = NULL;
}

void Tundra::Str::clear(Tundra::Str::String *str)
{
    str->num_chars = 0;
}
 
void Tundra::Str::add_char(Tundra::Str::String *str, char character)
{
    Tundra::Str::Internal::check_and_handle_resize(str);

    str->chars[str->num_chars - 1] = character;
    Tundra::Str::Internal::place_null_terminator(str);
}

void Tundra::Str::add_chars(Tundra::Str::String *str, const char* chars,
    Tundra::uint64 num_chars)
{
    // Add 1 to num_chars to account for null terminator.
    Tundra::reserve_mem((void**)&str->chars, num_chars, str->num_chars, 
        str->capacity);

    // Subtract 1 here to start the copying at the null terminator position.
    Tundra::copy_mem(chars, str->chars + str->num_chars - 1, num_chars);

    // Subtract 1 here so that when null terminator is placed it is placed right
    // at the end.
    str->num_chars += num_chars - 1;

    Tundra::Str::Internal::place_null_terminator(str);
}

void Tundra::Str::resize(Tundra::Str::String *str, Tundra::uint64 num_chars)
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

void Tundra::Str::shrink_to_capacity(Tundra::Str::String *str, 
    Tundra::uint64 capacity)
{
    // +1 to account for null terminator.
    if(capacity + 1 >= str->capacity) { return; }

    Tundra::Str::Internal::underlying_shrink(str, capacity);
}

void Tundra::Str::shrink_to_fit(Tundra::Str::String *str)
{
    if(str->num_chars == str->capacity) { return; }

    // Subtract 1 here since the underlying shrink method assumes the shrink
    // capacity does not include the null terminator count in the capacity. 
    Tundra::Str::Internal::underlying_shrink(str, str->num_chars - 1);
}

bool Tundra::Str::erase(Tundra::Str::String*str, Tundra::uint64 index)
{
    // Subtract 1 to not allowing erasure of the null terminator.
    if(index >= str->num_chars - 1) { return false; }

    Tundra::copy_mem(str->chars + index + 1, str->chars + index, 
        str->num_chars - index - 1);

    --str->num_chars;
    return true;
} 

char* Tundra::Str::back(Tundra::Str::String *str)
{
    return str->chars + str->num_chars - 1;
}

char* Tundra::Str::at_unchecked(Tundra::Str::String *str, Tundra::uint64 index)
{
    return str->chars + index;
}

char* Tundra::Str::at(Tundra::Str::String*str, Tundra::uint64 index)
{
    // Subtract 1 from num_chars to not allow indexing of the null terminator.
    return (index < str->num_chars - 1) ? str->chars + index : NULL;
}

const char* Tundra::Str::peek_unchecked(const Tundra::Str::String *str, 
    Tundra::uint64 index)
{
    return str->chars + index;
}

const char* Tundra::Str::peek(const Tundra::Str::String *str, 
    Tundra::uint64 index)
{
    // Subtract 1 from num_chars to not allow indexing of the null terminator.
    return (index < str->num_chars - 1) ? str->chars + index : NULL;
}

Tundra::uint64 Tundra::Str::size(const Tundra::Str::String *str)
{
    return str->num_chars - 1;
}

bool Tundra::Str::compare(const Tundra::Str::String *first,
    const Tundra::Str::String *second)
{
    if(first->num_chars != second->num_chars) { return false; }

    return Tundra::compare_mem((void*)first->chars, (void*)second->chars,
        first->num_chars);
}

Tundra::uint64 Tundra::Str::hash(const Tundra::Str::String *str)
{
    // Uses the FNV-1a hash method.

    Tundra::uint64 hash = 14695981039346656037ULL; // FNV offset basis
    const Tundra::uint64 FNV_PRIME = 1099511628211ULL; 

    for(Tundra::uint64 i = 0; i < str->num_chars - 1; ++i)
    {
        hash ^= str->chars[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

const char* Tundra::Str::data(const Tundra::Str::String *str)
{
    return str->chars;
}

