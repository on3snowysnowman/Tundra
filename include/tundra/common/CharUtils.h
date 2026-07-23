/**
 * @file CharUtils.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for reading certain properties of chars such as whether they 
 * are alphanumeric, are a digit, etc.
 * @version 0.1
 * @date 2026-07-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_CHARUTILS_H
#define TUNDRA_CHARUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns true if a character is a letter.
 * 
 * @param c Char.
 * 
 * @return `bool` True if letter. 
 */
static bool Tundra_is_letter(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

/**
 * @brief Returns true if a character is a digit.
 * 
 * @param c Char.
 * 
 * @return `bool` True if digit. 
 */
static bool Tundra_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 * @brief Returns true if a character is alphanumerical.
 * 
 * @param c Char.
 * 
 * @return `bool` True if alphanumerical. 
 */
static bool Tundra_is_alphnum(char c)
{
    return Tundra_is_letter(c) ||
        Tundra_is_digit(c);
}

#ifdef __cplusplus
}
#endif

#endif