/**
 * @file HashTable.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 08-01-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/Hash.hpp"

namespace Tundra::HshTbl
{

namespace Internal
{

template<typename key, Tundra::uint64 (*hash_func)(const key*)>
void check_hash_function()
{
}

} // namespace Internal

// Containers ------------------------------------------------------------------

template<typename key, typename value, 
    Tundra::uint64 (*hash_func)(const key*) = Tundra::hash<key>>
struct HashTable 
{
    
};


// Internal --------------------------------------------------------------------

namespace Internal
{

} // namespace Internal


// Public ---------------------------------------------------------------------

} // namespace Tundra::HshTbl
