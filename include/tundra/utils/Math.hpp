/**
 * @file Math.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic math functions, currently only min and max.
 * @version 0.1
 * @date 2025-10-21
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra
{

template<typename T>
constexpr T clamp_min(T num, T min)
{
    return (num < min) ? min : num; 
}

template<typename T>
constexpr T clamp_max(T num, T max)
{
    return (num > max) ? max : num;
}

/**
 * @brief Rounds `num` up to the smallest value that is aligned to `alignment`.
 *
 * @attention Ensure that `alignment` is a power of 2.
 * 
 * @tparam T Type of `num`.
 * @param num Number to round up.
 * @param alignment Alignment to round to.
 *
 * @return T `num` rounded up to the smallest increment of `alignment`. 
 */
template<typename T>
T round_to_alignment(T num, Tundra::uint64 pow2_value)
{
    return (num + (pow2_value - 1)) & ~(pow2_value - 1);
}

/**
 * @brief Computes 2 raised to the given exponent.
 *
 * @param exponent The exponent to raise 2 by.
 * @return Tundra::uint64 The result of 2^exponent.
 */
constexpr Tundra::uint64 pow2(Tundra::uint8 exponent)
{
    return 1ULL << exponent;
}

/**
 * @brief Computes num raised to the given exponent.
 *
 * @tparam T Type of the base number.
 * @param num The base number.
 * @param exponent The exponent to raise the base by.
 * @return Tundra::uint64 The result of num^exponent.
 */
template<typename T>
constexpr Tundra::uint64 pow(T base, Tundra::uint8 exponent)
{
    Tundra::uint64 result = 1;
    while (exponent) {
        if (exponent & 1) { result *= base; }
        base *= base;
        exponent >>= 1;
    }
    return result;
}

template<typename T>
constexpr Tundra::uint64 square(T num)
{
    return num * num;
}


} // namespace Tundra
