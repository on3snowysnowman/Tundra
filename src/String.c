/**
 * @file String.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/tundra_tools/String.h"

#include "tundra/tundra_tools/DynamicArray.h"


// Private Methods ------------------------------------------------------------




// Public Methods --------------------------------------------------------------

void Tundra_String_init(Tundra_String* string, uint64_t init_char_capacity)
{
    Tundra_DynamicArray_init(&string->chars, 1, init_char_capacity);
}

void Tundra_String_delete_string(Tundra_String* string)
{
    Tundra_DynamicArray_delete_array(&string->chars);
}

void Tundra_String_add_char(Tundra_String* string, char c)
{
    Tundra_DynamicArray_add_element(&string->chars, &c);
}

void Tundra_String_add_chars(Tundra_String* string, const char* chars,
    uint64_t num_chars)
{
    Tundra_DynamicArray_reserve(&string->chars, num_chars);

    for(uint64_t i = 0; i < num_chars; ++i)
    {
        Tundra_DynamicArray_add_element(&string->chars, chars + i);
    }
}

char Tundra_String_at(Tundra_String* string, uint64_t index)
{
    return *(char*)Tundra_DynamicArray_at(&string->chars, index);
}