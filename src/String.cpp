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
#include "tundra/utils/FatalHandler.hpp"


// Internal --------------------------------------------------------------------

bool Tundra::Str::Internal::underlying_init(Tundra::Str::String &str, 
    Tundra::uint64 init_capacity)
{
    str.chars = (char*)malloc(init_capacity);
    if(str.chars == nullptr) { return false; }
    
    str.num_chars = 0;
    str.capacity = init_capacity;

    return true;
}

void Tundra::Str::Internal::place_null_terminator(Tundra::Str::String &str)
{
    str.chars[str.num_chars] = '\0';
    ++str.num_chars;
}

bool Tundra::Str::Internal::check_and_handle_resize(Tundra::Str::String &str)
{
    if(str.num_chars < str.capacity) { return true; }

    // Get a new memory block that is twice the capacity of the current one.
    char *new_mem = (char*)malloc(
        str.capacity * 2);
    
    if(new_mem == nullptr) { return false; }

    Tundra::copy_mem_fwd(str.chars, new_mem, str.num_chars);

    ::free((void*)str.chars);
    // Tundra::free_aligned(str.chars);
    str.chars = new_mem;
    str.capacity *= 2;

    return true;
}

bool Tundra::Str::Internal::underlying_shrink(Tundra::Str::String &str, 
    Tundra::uint64 capacity)
{
    char *new_mem = (char*)Tundra::alloc_and_copy_mem((void*)str.chars, 
        capacity, capacity + 1);

    if(new_mem == nullptr) { return false; }

    // Tundra::free_aligned(str.chars);
    ::free(str.chars);
    str.chars = new_mem;
    str.num_chars = capacity;

    Tundra::Str::Internal::place_null_terminator(str);

    return true;
}


// Public ----------------------------------------------------------------------

bool Tundra::Str::reserve_for(Tundra::Str::String &str, 
    Tundra::uint64 extra_chars)
{
    str.capacity = Tundra::reserve_mem((void**)&str.chars,
        extra_chars, str.num_chars, str.capacity);

    return str.chars != nullptr;
}

bool Tundra::Str::init(Tundra::Str::String &str)
{
    bool init_success = Tundra::Str::Internal::underlying_init(str, 
        Tundra::Str::Internal::DEFAULT_CAPACITY);

    if(!init_success) { return false; }

    Tundra::Str::Internal::place_null_terminator(str);
    return true;
}

bool Tundra::Str::init(Tundra::Str::String &str, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (Tundra::uint64)(init_capacity == 0) *
        Tundra::Str::Internal::DEFAULT_CAPACITY;

    bool init_success = Tundra::Str::Internal::underlying_init(str, 
        init_capacity);
    
    if(!init_success) { return false; }

    Tundra::Str::Internal::place_null_terminator(str);
    return true;
}

bool Tundra::Str::init(Tundra::Str::String &str, const char* chars, 
    Tundra::uint64 num_chars)
{
    if(num_chars == 0)
    {
        return Tundra::Str::init(str);
    }

    // Add 1 here to account for the null terminator as well as the chars.
    Tundra::alloc_and_reserve_mem((void**)&str.chars, &str.capacity, 
        num_chars + 1);

    if(str.chars == nullptr) { return false; }

    Tundra::copy_mem_fwd(chars, str.chars, num_chars);

    str.num_chars = num_chars;

    Tundra::Str::Internal::place_null_terminator(str);

    return true;
}

void Tundra::Str::free(Tundra::Str::String &str)
{
    ::free(str.chars);
    str.chars = nullptr;
}

bool Tundra::Str::copy(Tundra::Str::String &dst, 
    const Tundra::Str::String &src)
{
    // Both objects are the same.
    if(&dst == &src) { return true; }

    if(dst.capacity != src.capacity || dst.chars == nullptr)
    {
        char *new_mem = (char*)malloc(src.capacity);
        if(new_mem == nullptr) { return false; }

        Tundra::Str::free(dst);
        dst.chars = new_mem;
        dst.capacity = src.capacity;
    }

    Tundra::copy_mem_fwd(src.chars, dst.chars, src.num_chars);
    dst.num_chars = src.num_chars;
    return true;
}

void Tundra::Str::move(Tundra::Str::String &dst, Tundra::Str::String &&src)
{
    // Both objects are the same.
    if(&dst == &src) { return; }

    Tundra::Str::free(dst);

    dst = src;
    src.chars = nullptr;
}

void Tundra::Str::clear(Tundra::Str::String &str)
{
    str.num_chars = 0;
    Tundra::Str::Internal::place_null_terminator(str);
}
 
bool Tundra::Str::add_char(Tundra::Str::String &str, char character)
{;
    if(!Tundra::Str::Internal::check_and_handle_resize(str)) { return false; }

    str.chars[str.num_chars - 1] = character;
    Tundra::Str::Internal::place_null_terminator(str);

    return true;
}

bool Tundra::Str::add_chars(Tundra::Str::String &str, const char* chars,
    Tundra::uint64 num_chars)
{
    // Add 1 to num_chars to account for null terminator.
    Tundra::reserve_mem((void**)&str.chars, num_chars, str.num_chars, 
        str.capacity);

    if(str.chars == nullptr) { return false; }

    // Subtract 1 here to start the copying at the null terminator position.
    Tundra::copy_mem_fwd(chars, str.chars + str.num_chars - 1, num_chars);

    // Subtract 1 here so that when null terminator is placed it is placed right
    // at the end.
    str.num_chars += num_chars - 1;

    Tundra::Str::Internal::place_null_terminator(str);

    return true;
}

bool Tundra::Str::resize(Tundra::Str::String &str, Tundra::uint64 num_chars)
{
    // Account for the null terminator.
    num_chars += 1;

    if(num_chars <= str.num_chars) { return true; }

    if(num_chars > str.num_chars)
    {
        Tundra::Str::reserve_for(str, num_chars - str.num_chars);

        if(str.chars == nullptr) { return false; }
    }

    // Deduct 1 to prepare the place_null_terminator method to put the 
    // terminator in the correct spot, which is one less than the capacity
    // we just resized.
    str.num_chars = num_chars - 1;
    Tundra::Str::Internal::place_null_terminator(str);

    return true;
}

bool Tundra::Str::shrink_to_capacity(Tundra::Str::String &str, 
    Tundra::uint64 capacity)
{
    // +1 to account for null terminator.
    if(capacity + 1 >= str.capacity) { return true; }

    return Tundra::Str::Internal::underlying_shrink(str, capacity);
}

bool Tundra::Str::shrink_to_fit(Tundra::Str::String &str)
{
    if(str.num_chars == str.capacity) { return true; }

    // Subtract 1 here since the underlying shrink method assumes the shrink
    // capacity does not include the null terminator count in the capacity. 
    return Tundra::Str::Internal::underlying_shrink(str, str.num_chars - 1);
}

bool Tundra::Str::erase(Tundra::Str::String &str, Tundra::uint64 index)
{
    // Subtract 1 to not allowing erasure of the null terminator.
    if(index >= str.num_chars - 1) { return false; }

    Tundra::copy_mem_fwd(str.chars + index + 1, str.chars + index, 
        str.num_chars - index - 1);

    --str.num_chars;
    return true;
} 

char* Tundra::Str::front(Tundra::Str::String &str)
{
    return str.chars;
}

const char* Tundra::Str::front(const Tundra::Str::String &str)
{
    return str.chars;
}

char* Tundra::Str::back(Tundra::Str::String &str)
{
    return str.chars + str.num_chars - 1;
}

const char* Tundra::Str::back(const Tundra::Str::String &str)
{
    return str.chars + str.num_chars - 1;
}

char& Tundra::Str::at_unchecked(Tundra::Str::String &str, Tundra::uint64 index)
{
    return str.chars[index];
}

char& Tundra::Str::at(Tundra::Str::String &str, Tundra::uint64 index)
{
    if(index < str.num_chars - 1) { return str.chars[index]; }
    
    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but String size is: \"%llu\".", index, 
        str.num_chars - 1);
}

const char& Tundra::Str::at(const Tundra::Str::String &str, 
    Tundra::uint64 index)
{
    if(index < str.num_chars - 1) { return str.chars[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but String size is: \"%llu\".", index, 
        str.num_chars - 1);
}

Tundra::uint64 Tundra::Str::size(const Tundra::Str::String &str)
{
    return str.num_chars - 1;
}

Tundra::uint64 Tundra::Str::capacity(const Tundra::Str::String &str)
{
    return str.capacity;
}

bool Tundra::Str::compare(const Tundra::Str::String *first,
    const Tundra::Str::String *second)
{
    if(first->num_chars != second->num_chars) { return false; }

    return Tundra::compare_mem((void*)first->chars, (void*)second->chars,
        first->num_chars);
}

Tundra::uint64 Tundra::Str::hash(const Tundra::Str::String &str)
{
    // Uses the FNV-1a hash method.

    Tundra::uint64 hash = 14695981039346656037ULL; // FNV offset basis
    const Tundra::uint64 FNV_PRIME = 1099511628211ULL; 

    for(Tundra::uint64 i = 0; i < str.num_chars - 1; ++i)
    {
        hash ^= static_cast<unsigned char>(str.chars[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}

const char* Tundra::Str::data(const Tundra::Str::String &str)
{
    return str.chars;
}

