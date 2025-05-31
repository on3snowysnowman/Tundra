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
 * This enum defines key identifiers that map to standard ASCII values.
 * It provides readable names for control keys, symbols, digits, and letters.
 *
 * All values are within the 8-bit ASCII range (0–127).
 *
 * Example: TUNDRA_A == 65 ('A'), TUNDRA_a == 97 ('a'), TUNDRA_RETURN == 13, 
 * etc.
 */
enum KEYCODE : uint8_t
{
    // Control characters
    TUNDRA_BACKSPACE = 8,
    TUNDRA_TAB,
    TUNDRA_RETURN = 13,    
    TUNDRA_ESCAPE = 27,
    TUNDRA_SPACE  = 32,

    // Symbols and punctuation
    TUNDRA_EXCLAMATION, 
    TUNDRA_QUOTE,
    TUNDRA_HASH,
    TUNDRA_DOLLAR,
    TUNDRA_PERCENT,
    TUNDRA_AMPERSAND,
    TUNDRA_APOSTROPHE,
    TUNDRA_LEFT_PAREN,
    TUNDRA_RIGHT_PAREN,
    TUNDRA_ASTERISK,
    TUNDRA_PLUS,
    TUNDRA_COMMA,
    TUNDRA_MINUS,
    TUNDRA_PERIOD,
    TUNDRA_SLASH,

    // Digits 0–9
    TUNDRA_0,
    TUNDRA_1,
    TUNDRA_2,
    TUNDRA_3,
    TUNDRA_4,
    TUNDRA_5,
    TUNDRA_6,
    TUNDRA_7,
    TUNDRA_8,
    TUNDRA_9,

    // More punctuation
    TUNDRA_COLON,
    TUNDRA_SEMICOLON,
    TUNDRA_LESS_THAN,
    TUNDRA_EQUAL,
    TUNDRA_GREATER_THAN,
    TUNDRA_QUESTION,
    TUNDRA_AT,

    // Uppercase letters
    TUNDRA_A,
    TUNDRA_B,
    TUNDRA_C,
    TUNDRA_D,
    TUNDRA_E,
    TUNDRA_F,
    TUNDRA_G,
    TUNDRA_H,
    TUNDRA_I,
    TUNDRA_J,
    TUNDRA_K,
    TUNDRA_L,
    TUNDRA_M,
    TUNDRA_N,
    TUNDRA_O,
    TUNDRA_P,
    TUNDRA_Q,
    TUNDRA_R,
    TUNDRA_S,
    TUNDRA_T,
    TUNDRA_U,
    TUNDRA_V,
    TUNDRA_W,
    TUNDRA_X,
    TUNDRA_Y,
    TUNDRA_Z,

    // More symbols
    TUNDRA_LEFT_BRACKET,
    TUNDRA_BACKSLASH,
    TUNDRA_RIGHT_BRACKET,
    TUNDRA_CARET,
    TUNDRA_UNDERSCORE,
    TUNDRA_GRAVE,

    // Lowercase letters
    TUNDRA_a,
    TUNDRA_b,
    TUNDRA_c,
    TUNDRA_d,
    TUNDRA_e,
    TUNDRA_f,
    TUNDRA_g,
    TUNDRA_h,
    TUNDRA_i,
    TUNDRA_j,
    TUNDRA_k,
    TUNDRA_l,
    TUNDRA_m,
    TUNDRA_n,
    TUNDRA_o,
    TUNDRA_p,
    TUNDRA_q,
    TUNDRA_r,
    TUNDRA_s,
    TUNDRA_t,
    TUNDRA_u,
    TUNDRA_v,
    TUNDRA_w,
    TUNDRA_x,
    TUNDRA_y,
    TUNDRA_z,

    // Final symbols
    TUNDRA_LEFT_BRACE,
    TUNDRA_PIPE,
    TUNDRA_RIGHT_BRACE,
    TUNDRA_TILDE,
    TUNDRA_DELETE,
    TUNDRA_INVALID_KEYCODE
};

};

