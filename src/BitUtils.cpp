/**
 * @file BitUtils.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for reading certain properties of bits, such as leading zeros.
 * @version 0.1
 * @date 2025-10-21
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "tundra/utils/BitUtils.hpp"


Tundra::uint8 Tundra::get_num_lead_zeros(Tundra::uint64 bits)
{
    return __builtin_clzll(bits);
}

Tundra::uint8 Tundra::get_num_trail_zeros(Tundra::uint64 bits)
{
    return __builtin_ctzll(bits);
}