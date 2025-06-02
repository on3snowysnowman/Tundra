/**
 * @file KeyCodes.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>


namespace Tundra
{

/**
 * @enum Tundra::KEYCODE
 * @brief ASCII-based keycode enum.
 *
 * Defines key identifiers that map to standard ASCII values.
 *
 * All values are within the 8-bit ASCII range (0–127).
 */
enum class KEYCODE : uint8_t
{
    // Invalid Key. Used internally for handling conversion of non supported 
    // GLFW keycodes.  
    INVALID_KEYCODE,

    BACKSPACE = 8,
    TAB,
    RETURN = 13,    
    ESCAPE = 27,
    SPACE  = 32,
    EXCLAMATION, 
    QUOTE,
    HASH,
    DOLLAR,
    PERCENT,
    AMPERSAND,
    APOSTROPHE,
    LEFT_PAREN,
    RIGHT_PAREN,
    ASTERISK,
    PLUS,
    COMMA,
    MINUS,
    PERIOD,
    SLASH,
    DIGIT_ZERO,
    DIGIT_ONE,
    DIGIT_TWO,
    DIGIT_THREE,
    DIGIT_FOUR,
    DIGIT_FIVE,
    DIGIT_SIX,
    DIGIT_SEVEN,
    DIGIT_EIGHT,
    DIGIT_NINE,
    COLON,
    SEMICOLON,
    LESS_THAN,
    EQUAL,
    GREATER_THAN,
    QUESTION,
    AT,
    UPPER_A,
    UPPER_B,
    UPPER_C,
    UPPER_D,
    UPPER_E,
    UPPER_F,
    UPPER_G,
    UPPER_H,
    UPPER_I,
    UPPER_J,
    UPPER_K,
    UPPER_L,
    UPPER_M,
    UPPER_N,
    UPPER_O,
    UPPER_P,
    UPPER_Q,
    UPPER_R,
    UPPER_S,
    UPPER_T,
    UPPER_U,
    UPPER_V,
    UPPER_W,
    UPPER_X,
    UPPER_Y,
    UPPER_Z,
    LEFT_BRACKET,
    BACKSLASH,
    RIGHT_BRACKET,
    CARET,
    UNDERSCORE,
    GRAVE,
    LOWER_A,
    LOWER_B,
    LOWER_C,
    LOWER_D,
    LOWER_E,
    LOWER_F,
    LOWER_G,
    LOWER_H,
    LOWER_I,
    LOWER_J,
    LOWER_K,
    LOWER_L,
    LOWER_M,
    LOWER_N,
    LOWER_O,
    LOWER_P,
    LOWER_Q,
    LOWER_R,
    LOWER_S,
    LOWER_T,
    LOWER_U,
    LOWER_V,
    LOWER_W,
    LOWER_X,
    LOWER_Y,
    LOWER_Z,
    LEFT_BRACE,
    PIPE,
    RIGHT_BRACE,
    TILDE,
    DELETE,
};

};

