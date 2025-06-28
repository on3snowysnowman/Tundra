/**
 * @file String.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Automatic resizing container for procedurally adding characters. 
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_STRING_H
#define TUNDRA_HGUARD_STRING_H

#include <stdint.h>

#include "tundra/tundra_tools/DynamicArray.h"

/**
 * @brief Generic expandable container for storing procedurally added 
 * characters.
 * 
 * Users should NOT modify internal variables, they are read-only.
 */
typedef struct Tundra_String 
{
    // Array of characters the string contains.
    Tundra_DynamicArray chars; 
}
Tundra_String;

/**
 * @brief Initializes heap memory for the String, allocating enough memory for 
 * `init_char_capacity` characters. 
 * 
 * If `init_char_capacity` is less than 2, a minimum capacity of 2 bytes is 
 * allocated. If the desire is to just have an emtpy created string with no 
 * memory allocated, do not initialize the String. Just be sure to call this 
 * method before performing any operations on it.
 * 
 * @param string String to initialize,
 * @param init_char_capacity Initial number of chars to allocate heap space for.
 */
void Tundra_Str_init(Tundra_String *string, uint64_t init_char_capacity);

/**
 * @brief Handles deletion of heap allocated memory for this String.
 * 
 * The String can be safely discarded after this method is called.
 * 
 * @param string String to delete.
 */
void Tundra_Str_delete_string(Tundra_String *string);

/**
 * @brief Adds a single character to the string.
 * 
 * @param string String to modify.
 * @param c Character to add
 */
void Tundra_Str_add_char(Tundra_String *string, char c);

/**
 * @brief Adds a sequence of chars to the string. 
 * 
 * The number of characters to add must be specified.
 * 
 * It is irrelevant if `chars` is null terminated or not, just ensure that if it
 * is, the terminator character is NOT included in final count of `num_chars`.
 * 
 * @param string String to modify.
 * @param chars Null terminated array of characters.
 * @param num_chars Number of characters in `chars`
 * @param 
 */
void Tundra_Str_add_chars(Tundra_String* string, const char* chars, 
    uint64_t num_chars);

/**
 * @brief Concatenates one String's contents to another. 
 * 
 * @param receiving_string String to add characters to.
 * @param donor_string String to grab content from.
 */
void Tundra_Str_concatenate_string(Tundra_String* receiving_string, 
    const Tundra_String* donor_string);

/**
 * @brief Returns the character at the passed index.
 * 
 * Performs bounds checking on the index.
 * 
 * @param string String to index.
 * @param index Index inside the String.
 */
char Tundra_Str_at(Tundra_String* string, uint64_t index);


#endif // TUNDRA_HGUARD_STRING_H