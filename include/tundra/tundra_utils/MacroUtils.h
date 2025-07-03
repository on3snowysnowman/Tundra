/**
 * @file MacroUtils.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Macro utilities for macro ease of use.
 * @version 0.1
 * @date 06-28-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_MACROUTILS_H
#define TUNDRA_HGUARD_MACROUTILS_H

#define TUNDRA_JOIN_TWO_MACROS_IMPL(a, b) a##b
#define TUNDRA_JOIN_TWO_MACROS(a, b) TUNDRA_JOIN_TWO_MACROS_IMPL(a, b)
#define TUNDRA_JOIN_THREE_MACROS(a, b, c) \
    TUNDRA_JOIN_TWO_MACROS(TUNDRA_JOIN_TWO_MACROS(a, b), c)

#define TUNDRA_DEFINE_FUNC_SIG(component_signature, \
    component_type, func_name) \
    TUNDRA_JOIN_TWO_MACROS(TUNDRA_JOIN_TWO_MACROS(component_signature, \
        component_type), func_name)

#endif // TUNDRA_HGUARD_MACROUTILS_H
