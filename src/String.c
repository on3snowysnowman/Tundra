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

void InTundra_Str_shrink(Tundra_String *str, uint64 cap)
{
    char *new_mem = (char*)Tundra_alloc_copy_mem(
        (const void*)str->chars,
        cap,
        cap);

    Tundra_free_mem((void*)str->chars);
    str->chars = new_mem;
    str->num_char = cap;
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
