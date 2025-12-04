/**
 * @file Tundra.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-17
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/internal/MemAllocHandler.hpp"
#include "tundra/utils/memory/MemAlloc.hpp"


namespace Tundra
{

/**
 * @brief Initializes any components in the library that need initializing. Must
 * be called before the library is used.
 * 
 * @return int 0 if successful. Error code otherwise.
 */
int init_tundra()
{
    Tundra::Internal::Mem::init();
    return 0;
}

};
