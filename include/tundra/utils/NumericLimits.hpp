/**
 * @file NumericLimits.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Maximum and Minimum value limits for numeric types.
 * @version 0.1
 * @date 10-07-25
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra 
{

namespace Internal
{
    constexpr Tundra::int8 INT8_MIN = -128;
    constexpr Tundra::int8 INT8_MAX = 127;
    constexpr Tundra::uint8 UINT8_MAX = 255U;

    constexpr Tundra::int16 INT16_MIN = -32768;
    constexpr Tundra::int16 INT16_MAX = 32767;
    constexpr Tundra::uint16 UINT16_MAX = 65535U;

    constexpr Tundra::int32 INT32_MIN = -2147483647 - 1;
    constexpr Tundra::int32 INT32_MAX = 2147483647;
    constexpr Tundra::uint32 UINT32_MAX = 4294967295U;

    constexpr Tundra::int64 INT64_MIN = -9223372036854775807LL - 1;
    constexpr Tundra::int64 INT64_MAX = 9223372036854775807LL;
    constexpr Tundra::uint64 UINT64_MAX = 18446744073709551615ULL;

}; // namespace Internal

template<typename T> 
struct NumericLimits {};

template<>
struct NumericLimits<Tundra::int8>
{   
    static constexpr Tundra::int8 min = Tundra::Internal::INT8_MIN;
    static constexpr Tundra::int8 max = Tundra::Internal::INT8_MAX;
};

template<>
struct NumericLimits<Tundra::uint8>
{
    static constexpr Tundra::uint8 min = 0;
    static constexpr Tundra::uint8 max = Tundra::Internal::UINT8_MAX;
};

template<>
struct NumericLimits<Tundra::int16>
{
    static constexpr Tundra::int16 min = Tundra::Internal::INT16_MIN;
    static constexpr Tundra::int16 max = Tundra::Internal::INT16_MAX;
};

template<>
struct NumericLimits<Tundra::uint16>
{
    static constexpr Tundra::uint16 min = 0;
    static constexpr Tundra::uint16 max = Tundra::Internal::UINT16_MAX;
};

template<>
struct NumericLimits<Tundra::int32>
{
    static constexpr Tundra::int32 min = Tundra::Internal::INT32_MIN;
    static constexpr Tundra::int32 max = Tundra::Internal::INT32_MAX;
};

template<>
struct NumericLimits<Tundra::uint32>
{
    static constexpr Tundra::uint32 min = 0;
    static constexpr Tundra::uint32 max = Tundra::Internal::UINT32_MAX;
};

template<>
struct NumericLimits<Tundra::int64>
{
    static constexpr Tundra::int64 min = Tundra::Internal::INT64_MIN;
    static constexpr Tundra::int64 max = Tundra::Internal::INT64_MAX;
};


template<>
struct NumericLimits<Tundra::uint64>
{
    static constexpr Tundra::uint64 min = 0;
    static constexpr Tundra::uint64 max = Tundra::Internal::UINT64_MAX;
};

} // namespace Tundra
