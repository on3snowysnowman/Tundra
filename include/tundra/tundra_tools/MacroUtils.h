/**
 * @file MacroUtils.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definition utilities for macro ease of use.
 * @version 0.1
 * @date 06-28-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef GUARDTUNDRA_MACROUTILS_H
#define GUARDTUNDRA_MACROUTILS_H

#define TUNDRA_JOIN_MACROS_IMPL(a, b) a##b
#define TUNDRA_JOIN_MACROS(a, b) TUNDRA_JOIN_MACROS_IMPL(a, b)

#define TUNDRA_DEFINE_FUNC_SIG(component_signature, \
    component_type, func_name) \
    TUNDRA_JOIN_MACROS(TUNDRA_JOIN_MACROS(component_signature, \
        component_type), func_name)

#define TUNDRA_DEFINE_INTFUNC_SIG(component_signature, \
    component_type, func_name) \
    TUNDRA_JOIN_MACROS(TUNDRA_JOIN_MACROS(component_signature, \
        component_type), func_name)

#endif // GUARDTUNDRA_MACROUTILS_H
