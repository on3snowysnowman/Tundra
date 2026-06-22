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
#include "tundra/utils/ConsoleIO.h"

static Tundra_fat_hand_t Tundra_fatal_func_handler_func = 
    &Tundra_dflt_fatal_func;

[[noreturn]] void InTundra_hard_trap(void)
{
    __builtin_trap();
}

[[noreturn]] void Tundra_dflt_fatal_func(
    const char* file, int line, const char* func, const char* fmt, 
    Tundra_VaList args)
{
    Tundra_print_fmt("[%s:%d in %s] -> ", file, line, func);
    Tundra_vargs_errprint_fmt(fmt, args);
    InTundra_hard_trap();
}

void Tundra_set_fatal_handler(Tundra_fat_hand_t handler)
{
    Tundra_fatal_func_handler_func = handler ? handler : 
        &Tundra_dflt_fatal_func;
}

void Tundra_fatal_func(const char* file, int line,
    const char* func, const char* fmt, ...)
{
    Tundra_VaList args;
    
    Tundra_varg_start(args, fmt);
    Tundra_fatal_func_handler_func(file, line, func, fmt, args);
    Tundra_varg_end(args);
}
