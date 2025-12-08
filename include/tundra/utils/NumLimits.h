/**
 * @file NumLimits.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Maximum and Minimum value limits for numeric types.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_NUMLIMITS_H
#define TUNDRA_NUMLIMITS_H

#define TUNDRA_INT8_MIN   (-128)
#define TUNDRA_INT8_MAX   (127)
#define TUNDRA_UINT8_MAX  (255U)    
#define TUNDRA_INT16_MIN  (-32768)
#define TUNDRA_INT16_MAX  (32767)
#define TUNDRA_UINT16_MAX (65535U)    
#define TUNDRA_INT32_MIN  (-2147483647 - 1)
#define TUNDRA_INT32_MAX  (2147483647)
#define TUNDRA_UINT32_MAX (4294967295U)    
#define TUNDRA_INT64_MIN  (-9223372036854775807LL - 1)
#define TUNDRA_INT64_MAX  (9223372036854775807LL)
#define TUNDRA_UINT64_MAX (18446744073709551615ULL)

#endif