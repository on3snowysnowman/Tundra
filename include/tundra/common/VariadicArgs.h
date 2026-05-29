/**
 * @file VariadicArgs.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Definitions for Variadic Arguments.
 * @version 0.1
 * @date 2026-05-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_VARIADICARGS_H
#define TUNDRA_VARIADICARGS_H

typedef __builtin_va_list Tundra_VaList;

#define Tundra_varg_start(args, last) __builtin_va_start(args, last)
#define Tundra_varg_arg(args, type) __builtin_va_arg(args, type)
#define Tundra_varg_end(args) __builtin_va_end(args)

#endif
