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
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/memory/MemUtils.hpp"


namespace Tundra::Str
{

// Internal --------------------------------------------------------------------

bool Internal::underlying_init(String &str, 
    Tundra::uint64 init_capacity)
{
    str.chars = reinterpret_cast<char*>(Tundra::alloc_mem(init_capacity));
    if(str.chars == nullptr) { return false; }
    
    str.num_chars = 0;
    str.capacity = init_capacity;

    return true;
}

void Internal::place_null_terminator(String &str)
{
    str.chars[str.num_chars] = '\0';
    ++str.num_chars;
}

bool Internal::check_and_handle_resize(String &str)
{
    if(str.num_chars < str.capacity) { return true; }

    // Get a new memory block that is twice the capacity of the current one.
    char *new_mem = reinterpret_cast<char*>(
        Tundra::alloc_mem(str.capacity * 2));
    
    if(new_mem == nullptr) { return false; }

    Tundra::copy_mem_fwd(str.chars, new_mem, str.num_chars);

    Tundra::free_mem(str.chars);
    // Tundra::free_aligned(str.chars);
    str.chars = new_mem;
    str.capacity *= 2;

    return true;
}

bool Internal::underlying_shrink(String &str, 
    Tundra::uint64 capacity)
{
    char *new_mem = reinterpret_cast<char*>(Tundra::alloc_copy_mem(str.chars, 
        capacity + 1, capacity));

    if(new_mem == nullptr) { return false; }

    Tundra::free_mem(str.chars);
    str.chars = new_mem;
    str.num_chars = capacity;

    Internal::place_null_terminator(str);

    return true;
}


// Public ----------------------------------------------------------------------

bool reserve_for(String &str, 
    Tundra::uint64 extra_chars)
{
    Tundra::reserve_mem(reinterpret_cast<void**>(&str.chars),
        &str.capacity, str.num_chars, extra_chars);

    return str.chars != nullptr;
}

bool init(String &str)
{
    bool init_success = Internal::underlying_init(str, 
        Internal::DEFAULT_CAPACITY);

    if(!init_success) { return false; }

    Internal::place_null_terminator(str);
    return true;
}

bool init(String &str, Tundra::uint64 init_capacity)
{
    // Set the initial capacity to the default if it is 0.
    init_capacity += (Tundra::uint64)(init_capacity == 0) *
        Internal::DEFAULT_CAPACITY;

    bool init_success = Internal::underlying_init(str, 
        init_capacity);
    
    if(!init_success) { return false; }

    Internal::place_null_terminator(str);
    return true;
}

bool init(String &str, const char* chars, 
    Tundra::uint64 num_chars)
{
    if(num_chars == 0)
    {
        return init(str);
    }

    // Add 1 here to account for the null terminator as well as the chars.
    Tundra::alloc_reserve_mem(reinterpret_cast<void**>(&str.chars), 
        &str.capacity, num_chars + 1);

    if(str.chars == nullptr) { return false; }

    Tundra::copy_mem_fwd(chars, str.chars, num_chars);

    str.num_chars = num_chars;

    Internal::place_null_terminator(str);

    return true;
}

void free(String &str)
{
    Tundra::free_mem(str.chars);
    str.chars = nullptr;
}

bool copy(String &dst, 
    const String &src)
{
    // Both objects are the same.
    if(&dst == &src) { return true; }

    if(dst.capacity != src.capacity || dst.chars == nullptr)
    {
        char *new_mem = reinterpret_cast<char*>(
            Tundra::alloc_mem(src.capacity));

        if(new_mem == nullptr) { return false; }

        free(dst);
        dst.chars = new_mem;
        dst.capacity = src.capacity;
    }

    Tundra::copy_mem_fwd(src.chars, dst.chars, src.num_chars);
    dst.num_chars = src.num_chars;
    return true;
}

void move(String &dst, String &&src)
{
    // Both objects are the same.
    if(&dst == &src) { return; }

    free(dst);

    dst = src;
    src.chars = nullptr;
}

void clear(String &str)
{
    str.num_chars = 0;
    Internal::place_null_terminator(str);
}
 
bool add_char(String &str, char character)
{;
    if(!Internal::check_and_handle_resize(str)) { return false; }

    str.chars[str.num_chars - 1] = character;
    Internal::place_null_terminator(str);

    return true;
}

bool add_chars(String &str, const char* chars,
    Tundra::uint64 num_chars)
{
    // Add 1 to num_chars to account for null terminator.
    Tundra::reserve_mem(reinterpret_cast<void**>(&str.chars), &str.capacity, 
        str.num_chars, num_chars);

    if(str.chars == nullptr) { return false; }

    // Subtract 1 here to start the copying at the null terminator position.
    Tundra::copy_mem_fwd(chars, str.chars + str.num_chars - 1, num_chars);

    // Subtract 1 here so that when null terminator is placed it is placed right
    // at the end.
    str.num_chars += num_chars - 1;

    Internal::place_null_terminator(str);

    return true;
}

bool resize(String &str, Tundra::uint64 num_chars)
{
    // Account for the null terminator.
    num_chars += 1;

    if(num_chars <= str.num_chars) { return true; }

    if(num_chars > str.num_chars)
    {
        reserve_for(str, num_chars - str.num_chars);

        if(str.chars == nullptr) { return false; }
    }

    // Deduct 1 to prepare the place_null_terminator method to put the 
    // terminator in the correct spot, which is one less than the capacity
    // we just resized.
    str.num_chars = num_chars - 1;
    Internal::place_null_terminator(str);

    return true;
}

bool shrink_to_capacity(String &str, 
    Tundra::uint64 capacity)
{
    // +1 to account for null terminator.
    if(capacity + 1 >= str.capacity) { return true; }

    return Internal::underlying_shrink(str, capacity);
}

bool shrink_to_fit(String &str)
{
    if(str.num_chars == str.capacity) { return true; }

    // Subtract 1 here since the underlying shrink method assumes the shrink
    // capacity does not include the null terminator count in the capacity. 
    return Internal::underlying_shrink(str, str.num_chars - 1);
}

bool erase(String &str, Tundra::uint64 index)
{
    // Subtract 1 to not allowing erasure of the null terminator.
    if(index >= str.num_chars - 1) { return false; }

    Tundra::copy_mem_fwd(str.chars + index + 1, str.chars + index, 
        str.num_chars - index - 1);

    --str.num_chars;
    return true;
} 

char* front(String &str)
{
    return str.chars;
}

const char* front(const String &str)
{
    return str.chars;
}

char* back(String &str)
{
    return str.chars + str.num_chars - 1;
}

const char* back(const String &str)
{
    return str.chars + str.num_chars - 1;
}

char& at_unchecked(String &str, Tundra::uint64 index)
{
    return str.chars[index];
}

char& at(String &str, Tundra::uint64 index)
{
    if(index < str.num_chars - 1) { return str.chars[index]; }
    
    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but String size is: \"%llu\".", index, 
        str.num_chars - 1);
}

const char& at(const String &str, 
    Tundra::uint64 index)
{
    if(index < str.num_chars - 1) { return str.chars[index]; }

    // Invalid index.
    TUNDRA_FATAL("Index is: \"%llu\" but String size is: \"%llu\".", index, 
        str.num_chars - 1);
}

Tundra::uint64 size(const String &str)
{
    return str.num_chars - 1;
}

Tundra::uint64 capacity(const String &str)
{
    return str.capacity;
}

bool compare(const String *first,
    const String *second)
{
    if(first->num_chars != second->num_chars) { return false; }

    return Tundra::cmp_mem(first->chars, second->chars, first->num_chars);
}

Tundra::uint64 hash(const String &str)
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

const char* data(const String &str)
{
    return str.chars;
}


// Iterator Methods ------------------------------------------------------------

Iterator begin(String &str)
{
    return Iterator {str.chars};
}

Iterator end(String &str)
{
    return Iterator {str.chars + str.num_chars};
}

bool operator==(const Iterator &first, const Iterator &second)
{
    return first.data == second.data;
}

Iterator& operator++(Iterator &it)
{
    ++it.data;
    return it;
}

Iterator operator++(Iterator &it, int /** postfix */)
{
    Iterator copy = it;
    ++it;
    return copy;
}

Iterator& operator--(Iterator &it)
{
    --it.data;
    return it;
}

Iterator operator--(Iterator &it, int /** postfix */)
{
    Iterator copy = it;
    --it;
    return copy;
}

char& operator*(const Iterator &it)
{
    return *it.data;
}


} // namespace Tundra::Str

