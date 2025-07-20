/**
 * @file string.h
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
#include <stdbool.h>

/**
 * @brief Generic expandable container for storing procedurally added 
 * characters.
 * 
 * Internal variables are read-only.
 */
typedef struct Tundra_String 
{
    // Array of characters the string contains.
    char* data;

    // Number of characters currently stored in the string, EXCLUDING the null
    // terminator.
    uint64_t num_chars;

    // Max number of chars the string can currently hold before needing to 
    // resize.
    uint64_t capacity;
}
Tundra_String;

/**
 * @brief Initializes heap memory for the string, allocating two initial bytes
 * of memory.
 * 
 * This is the default init method that simply allocates just 2 bytes of memory.
 * If the size of initial bytes is desired to be specified, use the 
 * `Tundra_Str_init_w_size` method.
 * 
 * @attention A string must be initialized using any of the `_init...` methods
 * before use. Strings must only be initialized once before they are 
 * deconstructed. After deconstruction, they may be initialized again.
 * 
 * @param string 
 */
void  Tundra_Str_init(Tundra_String *string);

/**
 * @brief Initializes heap memory for the string, allocating enough memory for 
 * `init_char_capacity` characters. 
 * 
 * If `init_char_capacity` is set as 0, a capacity of 2 characters is 
 * allocated (1 character space available, as a byte is taken up by the null 
 * terminator). If the desire is to just have an empty created string with no 
 * memory allocated, do not initialize the string until it needs to be used.
 * 
 * @attention A string must be initialized using any of the `_init...` methods
 * before use. Strings must only be initialized once before they are 
 * deconstructed. After deconstruction, they may be initialized again.
 * 
 * @param string string to initialize,
 * @param init_char_capacity Initial number of chars to allocate heap space for.
 */
void Tundra_Str_init_w_size(Tundra_String *string, uint64_t init_char_capacity);

/**
 * @brief Initializes heap memory for the string, allocating enough memory for
 * `size_chars` chars. The characters of `chars` are copied into the string.
 * 
 * The number of characters in `chars` must be specified.
 * 
 * It is irrelevant if `chars` is null terminated or not, just ensure that if it
 * is, the terminator character is NOT included in final count of `size_chars`.
 * 
 * @attention A string must be initialized using any of the `_init...` methods
 * before use. Strings must only be initialized once before they are 
 * deconstructed. After deconstruction, they may be initialized again.
 * 
 * @param string String to initialize.
 * @param chars Array of characters to copy. 
 * @param size_chars Number of characters in `chars`.
 */
void Tundra_Str_init_w_chars(Tundra_String *string, const char* chars,
    uint64_t size_chars);

/**
 * @brief Handles deletion of heap allocated memory for this string.
 * 
 * The string can be safely discarded after this method is called.
 * 
 * @param string string to delete.
 */
void Tundra_Str_deconstruct(Tundra_String *string);

/**
 * @brief Adds a single character to the string.
 * 
 * @param string string to modify.
 * @param c Character to add
 */
void Tundra_Str_add_char(Tundra_String *string, char c);

/**
 * @brief Adds a sequence of chars to the string. 
 * 
 * The number of characters in `chars` must be specified.
 * 
 * It is irrelevant if `chars` is null terminated or not, just ensure that if it
 * is, the terminator character is NOT included in final count of `size_chars`.
 * 
 * @param string string to modify.
 * @param chars Array of characters.
 * @param size_chars Number of characters in `chars`.
 */
void Tundra_Str_add_chars(Tundra_String *string, const char* chars, 
    uint64_t size_chars);

/**
 * @brief Concatenates one string's contents to another. 
 * 
 * @param receiving_string string to add characters to.
 * @param donor_string string to grab content from.
 */
void Tundra_Str_concatenate_string(Tundra_String *receiving_string, 
    const Tundra_String *donor_string);

/**
 * @brief Given two strings, returns true if their characters are identical.
 * 
 * @param a First string
 * @param b Second string.
 */
bool Tundra_Str_compare(Tundra_String *a, Tundra_String *b);

/**
 * @brief Returns a pointer to the character at the passed index.
 * 
 * Performs bounds checking on the index.
 * 
 * @param string string to index.
 * @param index Index inside the string.
 */
char* Tundra_Str_at(Tundra_String *string, uint64_t index);

/**
 * @brief Returns the number of characters the string currently holds.
 * 
 * This does NOT include the null terminator character.
 * 
 * @param string String to analyze.
 */
uint64_t Tundra_Str_size(Tundra_String *string);


#endif // TUNDRA_HGUARD_STRING_H