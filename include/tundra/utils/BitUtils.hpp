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

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/Math.hpp"


namespace Tundra
{

constexpr Tundra::uint64 KIBIBYTE = Tundra::pow2(10);
constexpr Tundra::uint64 MEBIBYTE = Tundra::pow2(20);
constexpr Tundra::uint64 GIBIBYTE = Tundra::pow2(30);

constexpr Tundra::uint8 get_num_lead_zeros(Tundra::uint64 bits)
{
    return __builtin_clzll(bits);
}

constexpr Tundra::uint8 get_num_trail_zeros(Tundra::uint64 bits)
{
    return __builtin_ctzll(bits);
}

constexpr Tundra::uint8 get_msb_pos(Tundra::uint64 bits)
{
    return 63ULL - get_num_lead_zeros(bits);
}

} // namespace Tundra
