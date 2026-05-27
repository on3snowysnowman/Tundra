/**
 * @file Core.h
 * @author your name (you@domain.com)
 * @brief Standard methods such as init, assert and exit.
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/common/TypeDef.h"

#define TUNDRA_CMPDIR_NORETURN __attribute__((noreturn))

#define TUNDRA_CMPDIR_ALIGN(alignment) \
    __attribute__((aligned(alignment)))

#define TUNDRA_CT_ASSERT(cond) typedef char \
    TUNDRA_CONCAT(TUNDRA_CT_ASSERTION, __COUNTER__)[(cond) ? 1 : -1]

#define TUNDRA_RT_ASSERT(expr, msg, ...) \
    ((expr) ? (void)(0) : TUNDRA_FATAL(msg, __VA_ARGS__))

TUNDRA_CMPDIR_NORETURN void Tundra_exit(i64 exit_code);