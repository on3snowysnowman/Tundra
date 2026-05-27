/**
 * @file FatalHandler.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Handler for when fatal crashes are reached, outputs a 
 * message and runs the fatal handling function.
 * @version 0.1
 * @date 2026-05-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/utils/FatalHandler.h"
#include "tundra/utils/ConsoleOutput.h"

static Tundra_fat_hand_t Tundra_FtlHnd_fatal_handler_func = 
    &Tundra_FtlHnd_default_fatal_handler;

[[noreturn]] void InTundra_FtlHnd_hard_trap(void)
{
    __builtin_trap();
}

[[noreturn]] void Tundra_FtlHnd_default_fatal_handler(
    const char* file, int line, const char* func, const char* fmt, 
    Tundra_VaList args)
{
    Tundra_printf("[%s:%d in %s] -> ", file, line, func);
    Tundra_vargs_printf(fmt, args);
    InTundra_FtlHnd_hard_trap();
}

void Tundra_FtlHnd_set_fatal_handler(Tundra_fat_hand_t handler)
{
    Tundra_FtlHnd_fatal_handler_func = handler ? handler : 
        &Tundra_FtlHnd_default_fatal_handler;
}

void Tundra_FtlHnd_fatal(const char* file, int line,
    const char* func, const char* fmt, ...)
{
    Tundra_VaList args;
    
    Tundra_varg_start(args, fmt);
    Tundra_FtlHnd_fatal_handler_func(file, line, func, fmt, args);
    Tundra_varg_end(args);
}
