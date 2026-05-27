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

#include "tundra/common/Core.h"
#include "tundra/internal/Syscall.h"

TUNDRA_CMPDIR_NORETURN void Tundra_exit(i64 exit_code)
{
    InTundra_syscall(TUNDRA_LINUX_SYSCALL_EXIT, exit_code, 0, 0, 0, 0, 0);

    for(;;) {}
}
