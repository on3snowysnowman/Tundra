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

#include "tundra/tundra_utils/String.h"

#include <stdlib.h>
#include <string.h>

#include "tundra/tundra_utils/Memory.h"


// Public Methods --------------------------------------------------------------

inline void Tundra_Str_init(Tundra_String *string)
{
    string->data = malloc(2);

    string->capacity = 2;
    string->num_chars = 0;

    string->data[0] = '\0';
}

void Tundra_Str_init_w_size(Tundra_String *string, uint64_t init_char_capacity)
{
    // Add two to the initial capacity if it is 0.
    init_char_capacity += 2 * (init_char_capacity == 0);

    string->data = malloc(init_char_capacity);

    string->capacity = init_char_capacity;
    string->num_chars = 0;

    // Place initial null terminator.
    string->data[0] = '\0';
}

void Tundra_Str_init_w_chars(Tundra_String *string, const char* chars, 
    uint64_t size_chars)
{
    // Add 1 to size_chars to account for the null terminator.
    string->data = malloc(size_chars + 1);
    
    string->capacity = size_chars + 1;
    string->num_chars += size_chars;

    // Copy characters from `chars`.
    memcpy(string->data, chars, size_chars);

    // Place initial null terminator
    string->data[string->num_chars] = '\0';
}

inline void Tundra_Str_deconstruct(Tundra_String *string)
{
    free(string->data);
    string->data = NULL;
}

void Tundra_Str_add_char(Tundra_String *string, char c)
{
    // Capacity of the string has been reached, need to allocate more memory.
    // Add 1 to the num_chars here to account for the null terminator.
    if(string->num_chars + 1 >= string->capacity) 
    {
        // Ignore copying of the null terminator here since we're going to 
        // immediately overwrite it anyway.
        void* new_memory = Tundra_alloc_and_copy_memory(string->data, 
            string->num_chars, string->capacity * 2);

        free(string->data);

        string->data = new_memory;

        // Update the string's capacity to its new value.
        string->capacity *= 2;
    }

    // Set the current null terminator, which is at the end of the string to the
    // new char.
    string->data[string->num_chars] = c;

    // Place null terminator 
    string->data[++string->num_chars] = '\0';
}

void Tundra_Str_add_chars(Tundra_String *string, const char* char_array,
    uint64_t size_chars)
{
    // Add 1 to num_chars here to account for the memory taken up by the null
    // terminator.
    Tundra_reserve_memory((void**)(&string->data), size_chars, 
        string->num_chars + 1, string->capacity);

    memcpy(string->data + string->num_chars, char_array, size_chars);

    // Update size of the string since we've just placed chars.
    string->num_chars += size_chars;

    // Replace null terminator at the end of the string.
    string->data[string->num_chars] = '\0';
}

bool Tundra_Str_compare(Tundra_String* a, Tundra_String* b)
{
    // If string sizes are not equal, they can't possibly have the same
    // contents.
    if(a->num_chars != b->num_chars) return false;

    for(uint64_t i = 0; i < a->num_chars; ++i)
    {
        if(a->data[i] != b->data[i]) return false; 
    }

    return true;
}

inline char* Tundra_Str_at(Tundra_String *string, uint64_t index)
{
    // Subtract 1 here to account for not accessing the null terminator.
    if(index >= string->num_chars) return NULL;

    return string->data + index;
}

inline uint64_t Tundra_Str_size(Tundra_String *string)
{
    return string->num_chars;
}
