/**
 * @file String.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for procedurally adding characters to a
 *    null terminated array.
 * @date 2025-12-06
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/containers/String.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/MemUtils.h"
#include "tundra/utils/FatalHandler.h"


// Internal Methods ------------------------------------------------------------

void InTundra_Str_init(Tundra_String *str, uint64 init_cap)
{
    str->chars = (char*)Tundra_alloc_mem(init_cap);

    str->num_char = 0;
    str->cap = init_cap;

    InTundra_Str_place_null_t(str);
}

void InTundra_Str_place_null_t(Tundra_String *str)
{
    str->chars[str->num_char] = '\0';
    ++str->num_char;
}

void InTundra_Str_check_handle_exp(Tundra_String *str)
{
    if(str->num_char < str->cap) { return; }

    const uint64 NEW_CAP = str->cap * 2;

    // Double previous capacity.
    char *new_mem = (char*)Tundra_alloc_mem(NEW_CAP);

    Tundra_copy_mem_fwd((const void*)str->chars, (void*)new_mem, str->num_char);
    Tundra_free_mem((void*)str->chars);

    str->chars = new_mem;
    str->cap = NEW_CAP;
}

void InTundra_Str_reserve_for(Tundra_String *str, uint64 extra_chars)
{
    const uint64 TOT_REQ_BYTE = str->num_char + extra_chars;

    // Calculate new capacity by doubling current capacity until the required
    // bytes are reached.
    const uint64 NEW_CAP_BYTE = InTundra_calc_new_capacity_by_doubling(
        TOT_REQ_BYTE, str->cap);

    // Allocate new memory and copy existing elements over.
    char *new_mem = (char*)Tundra_alloc_copy_mem(
        (const void*)str->chars,
        NEW_CAP_BYTE,
        str->num_char);

    // Free old memory.
    Tundra_free_mem((void*)str->chars);
    str->chars = new_mem;
    str->cap = NEW_CAP_BYTE;
}

#include <stdio.h>
void InTundra_Str_shrink(Tundra_String *str, uint64 cap)
{
    char *new_mem = (char*)Tundra_alloc_copy_mem(
        (const void*)str->chars,
        cap,
        cap - 1); // -1 since we're replacing it with a null terminator.
    new_mem[cap - 1] = '\0'; // Place null terminator.

    Tundra_free_mem((void*)str->chars);
    str->chars = new_mem;
    str->num_char = cap;
    str->cap = cap;
}


// Public Methods -------------------------------------------------------------

void Tundra_Str_init(Tundra_String *str)
{
    InTundra_Str_init(str, TUNDRA_STR_DEF_CAP);
}

void Tundra_Str_init_w_cap(Tundra_String *str, uint64 init_cap)
{
    init_cap = (init_cap == 0) ? TUNDRA_STR_DEF_CAP : init_cap; 

    InTundra_Str_init(str, init_cap);
}

void Tundra_Str_init_w_chars(Tundra_String *str, const char *chars, 
    uint64 num_chars, bool strict_alloc)
{
    const uint64 CAP = num_chars + 1; // +1 for null terminator.

    // Allocate exactly enough bytes for the memory to copy in.
    if(strict_alloc)
    {
        str->chars = (char*)Tundra_alloc_mem(CAP);
        Tundra_copy_mem_fwd((const void*)chars, (void*)str->chars, num_chars);
        str->num_char = num_chars;
        str->cap = CAP; 
        InTundra_Str_place_null_t(str);
        return;
    }

    // -- Use the "reserving" method to alloc, which will generally alloc more
    // space than is needed to prevent immediate expansion on next add call. --

    Tundra_alloc_reserve_mem(
        (void**)(&str->chars),
        &str->cap,
        CAP); 

    Tundra_copy_mem_fwd((const void*)chars, (void*)str->chars, num_chars);

    str->num_char = num_chars;
    InTundra_Str_place_null_t(str);
}

void Tundra_Str_free(Tundra_String *str)
{
    Tundra_free_mem((void*)str->chars);
    str->chars = NULL;
}

void Tundra_Str_copy(const Tundra_String *src, Tundra_String *dst)
{
    if(src == dst) { return; }

    if(dst->cap != src->cap)
    {
        Tundra_free_mem((void*)dst->chars);
        dst->chars = (char*)(Tundra_alloc_mem(src->cap));
        dst->cap = src->cap;
    }

    Tundra_copy_mem_fwd(
        (const void*)src->chars,
        (void*)dst->chars,
        src->num_char);
    dst->num_char = src->num_char;
}

void Tundra_Str_move(Tundra_String *src, Tundra_String *dst)
{
    if(src == dst) { return; }

    Tundra_free_mem((void*)dst->chars);
    *dst = *src;
    src->chars = NULL;
}

void Tundra_Str_clear(Tundra_String *str)
{
    str->num_char = 0;
    InTundra_Str_place_null_t(str);
}

void Tundra_Str_add(Tundra_String *str, char ch)
{
    InTundra_Str_check_handle_exp(str);

    str->chars[str->num_char - 1] = ch; // -1 to overwrite null terminator.
    InTundra_Str_place_null_t(str);
}

void Tundra_Str_add_multiple(Tundra_String *str, const char *chars, 
    uint64 num_char)
{
    if(str->cap - str->num_char < num_char)
    {
        InTundra_Str_reserve_for(str, num_char);
    }

    Tundra_copy_mem_fwd(
        (const void*)chars,
        (void*)(str->chars + str->num_char - 1), 
            // -1 to overwrite null terminator.
        num_char);

    str->num_char += num_char - 1; 
        // -1 to account for overwritten null terminator, num_char will be added 
        // back in InTundra_Str_place_null_t.
    InTundra_Str_place_null_t(str);
}

void Tundra_Str_insert(Tundra_String *str, char ch, uint64 index)
{
    if(index >= str->num_char)
    {
        TUNDRA_FATAL("Tundra_Str_insert: Insert index %llu out of bounds "
            "for String of size %llu.", index, str->num_char);
    }

    InTundra_Str_check_handle_exp(str);

    // Move chars forward 1 from index to end.
    Tundra_copy_mem_bwd(
        (const void*)(str->chars + index),
        (void*)(str->chars + index + 1),
        (str->num_char - index) * sizeof(char));

    str->chars[index] = ch;
    ++str->num_char;
}

void Tundra_Str_resize(Tundra_String *str, uint64 num_char)
{
    const uint64 NUM_CHAR_W_NULL = num_char + 1; // +1 for null terminator.

    if(NUM_CHAR_W_NULL == str->num_char) { return; } // +1 for null terminator.

    if(NUM_CHAR_W_NULL < str->num_char)
    {
        str->num_char = num_char;
        InTundra_Str_place_null_t(str);
        return;
    }

    // -- NUM_CHAR_W_NULL > str->num_char --

    const uint64 NEW_CAP = InTundra_calc_new_capacity_by_doubling(
        NUM_CHAR_W_NULL, str->cap);

    char *new_mem = (char*)Tundra_alloc_copy_mem(
        (const void*)str->chars,
        NEW_CAP,
        str->num_char);

    Tundra_free_mem((void*)str->chars);
    str->chars = new_mem;
    str->cap = NEW_CAP;

    str->num_char = num_char;
    InTundra_Str_place_null_t(str);
}

void Tundra_Str_reserve(Tundra_String *str, uint64 extra_chars)
{
    if(str->cap - str->num_char >= extra_chars) { return; }

    InTundra_Str_reserve_for(str, extra_chars);
}

void Tundra_Str_shrink_to_new_cap(Tundra_String *str, uint64 new_cap)
{
    const uint64 CAP_W_NULL = new_cap + 1; // +1 for null terminator.

    if(CAP_W_NULL >= str->cap) { return; }

    InTundra_Str_shrink(str, CAP_W_NULL);   
}

void Tundra_Str_shrink_to_fit(Tundra_String *str)
{
    if(str->num_char == str->cap) { return; }
    
    InTundra_Str_shrink(str, str->num_char);
}

void Tundra_Str_erase(Tundra_String *str, uint64 index)
{
    if(index >= str->num_char - 1) // -1 to avoid removing null terminator.
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for "
            "String of size \"%llu.\"", index, str->num_char);
    }

    // Shift characters after index back by one.
    Tundra_erase_shift_left(
        (void*)str->chars,
        index,
        1, // Size of type char is 1 byte.
        str->num_char);

    --str->num_char;
}

char* Tundra_Str_front(Tundra_String *str)
{
    return &str->chars[0];
}

const char* Tundra_Str_front_cst(const Tundra_String *str)
{
    return &str->chars[0];
}

char* Tundra_Str_back(Tundra_String *str)
{
    return &str->chars[str->num_char - 2]; // -2 to avoid null terminator.
}

const char* Tundra_Str_back_cst(const Tundra_String *str)
{
    return &str->chars[str->num_char - 2]; // -2 to avoid null terminator.
}

char* Tundra_Str_at_nocheck(Tundra_String *str, uint64 index)
{
    return &str->chars[index];
}

const char* Tundra_Str_at_nocheck_cst(const Tundra_String *str, uint64 index)
{
    return &str->chars[index];
}

char* Tundra_Str_at(Tundra_String *str, uint64 index)
{
    if(index >= str->num_char - 1) // -1 to avoid null terminator.
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for "
            "String of size \"%llu.\"", index, str->num_char);
    }

    return &str->chars[index];
}

const char* Tundra_Str_at_cst(const Tundra_String *str, uint64 index)
{
    if(index >= str->num_char - 1) // -1 to avoid null terminator.
    {
        TUNDRA_FATAL("Index \"%llu\" out of bounds for "
            "String of size \"%llu.\"", index, str->num_char);
    }

    return &str->chars[index];
}

const char* Tundra_Str_data(const Tundra_String *str)
{
    return (const char*)str->chars;
}

uint64 Tundra_Str_size(const Tundra_String *str)
{
    return str->num_char - 1; // -1 to not count null terminator.
}

uint64 Tundra_Str_capacity(const Tundra_String *str)
{
    return str->cap - 1; // -1 to not count null terminator.
}
