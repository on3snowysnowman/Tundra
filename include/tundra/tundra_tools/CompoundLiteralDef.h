/**
 * @file CompoundLiteralDef.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for creating pointers to temporary literals.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_COMPOUNDLITERALDEF_H
#define TUNDRA_HGUARD_COMPOUNDLITERALDEF_H

#define TUNDRA_TEMP_INT(val) ((int[]){val})
#define TUNDRA_TEMP_FLOAT(val) ((float[]){val})
#define TUNDRA_TEMP_CHAR(val) ((char[]){val})
#define TUNDRA_TEMP_OF_TYPE(type, val) ((type[]){val})

#endif // TUNDRA_HGUARD_COMPOUNDLITERALDEF_H
