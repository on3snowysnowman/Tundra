/**
 * @file MacroHelper.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple Macros for helping with Macro expansion and creation.
 * @version 0.1
 * @date 07-30-25
 *
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_MACROHELPER_H
#define TUNDRA_MACROHELPER_H

#define TUNDRA_EXPAND(a) a
#define TUNDRA_CONCAT_HELPER(a, b) a##b
#define TUNDRA_CONCAT(a, b) TUNDRA_CONCAT_HELPER(a, b)
#define TUNDRA_CONCAT3(a, b, c) TUNDRA_CONCAT(TUNDRA_CONCAT(a, b), c)
#define TUNDRA_CONCAT4(a, b, c, d) TUNDRA_CONCAT(TUNDRA_CONCAT3(a, b, c), d)

#endif
