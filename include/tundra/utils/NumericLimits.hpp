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
#include "tundra/utils/TypeCheck.hpp"


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
struct NumericLimits 
{
    static_assert(false, "No NumericLimits template for the provided type");
};

template<>
struct NumericLimits<Tundra::int8>
{   
    static constexpr Tundra::int8 min = Tundra::Internal::INT8_MIN;
    static constexpr Tundra::int8 max = Tundra::Internal::INT8_MAX;
    static constexpr Tundra::uint8 digits = 8;
};

template<>
struct NumericLimits<Tundra::uint8>
{
    static constexpr Tundra::uint8 min = 0;
    static constexpr Tundra::uint8 max = Tundra::Internal::UINT8_MAX;
    static constexpr Tundra::uint8 digits = 8;
};

template<>
struct NumericLimits<Tundra::int16>
{
    static constexpr Tundra::int16 min = Tundra::Internal::INT16_MIN;
    static constexpr Tundra::int16 max = Tundra::Internal::INT16_MAX;
    static constexpr Tundra::uint8 digits = 16;
};

template<>
struct NumericLimits<Tundra::uint16>
{
    static constexpr Tundra::uint16 min = 0;
    static constexpr Tundra::uint16 max = Tundra::Internal::UINT16_MAX;
    static constexpr Tundra::uint8 digits = 16;
};

template<>
struct NumericLimits<Tundra::int32>
{
    static constexpr Tundra::int32 min = Tundra::Internal::INT32_MIN;
    static constexpr Tundra::int32 max = Tundra::Internal::INT32_MAX;
    static constexpr Tundra::uint8 digits = 32;
};

template<>
struct NumericLimits<Tundra::uint32>
{
    static constexpr Tundra::uint32 min = 0;
    static constexpr Tundra::uint32 max = Tundra::Internal::UINT32_MAX;
    static constexpr Tundra::uint8 digits = 32;
};

template<>
struct NumericLimits<Tundra::int64>
{
    static constexpr Tundra::int64 min = Tundra::Internal::INT64_MIN;
    static constexpr Tundra::int64 max = Tundra::Internal::INT64_MAX;
    static constexpr Tundra::uint8 digits = 64;
};


template<>
struct NumericLimits<Tundra::uint64>
{
    static constexpr Tundra::uint64 min = 0;
    static constexpr Tundra::uint64 max = Tundra::Internal::UINT64_MAX;
    static constexpr Tundra::uint8 digits = 64;
};


/**
 * @brief Returns true if `num` will overflow when `amount` is added to it. 
 * 
 * T must be an integral type.
 *
 * @tparam T Type of the numbers being summed.
 * 
 * @param num Number to check for overflow. 
 * @param amount Amount to add to `num`.
 * 
 * @return bool True if `num` will overflow.
 */
template<typename T> 
inline bool will_add_overflow(T num, T amount)
{
    T sink;
    return __builtin_add_overflow(num, amount, &sink);
}

/**
 * @brief Adds `amount` to `num`, returning true if there was an overflow.
 *
 * `num` is modified directly through pass by reference. If overflow occurs, 
 * `num`'s value is indeterminate and cannot be guaranteed to hold an expected
 * value.
 *
 * `num` and `amount` must both be integral types.
 * 
 * @tparam T1 Type of `num`.
 * @tparam T2 Type of `amount`.
 *
 * @param num Num to add to. 
 * @param amount Amount to add.
 *
 * @return bool True if overflow has occurred.  
 */
template<typename T1, typename T2>
inline bool add_check_overflow(T1& num, T2 amount)
{
    static_assert(Tundra::is_integral_type<T1>::value, "T1 must be an integral \
        type.");
    static_assert(Tundra::is_integral_type<T2>::value, "T2 must be an integral \
        type.");

    return __builtin_add_overflow(num, amount, &num);
}

/**
 * @brief Subtracts `amount` from `num`, returning true if there was an overflow.
 *
 * `num` is modified directly through pass by reference. If overflow occurs, 
 * `num`'s value is indeterminate and cannot be guaranteed to hold an expected
 * value.
 *
 * `num` and `amount` must both be integral types.
 * 
 * @tparam T1 Type of `num`.
 * @tparam T2 Type of `amount`.
 *
 * @param num Num to subtract from. 
 * @param amount Amount to subtract.
 *
 * @return bool True if overflow has occurred.  
 */
template<typename T1, typename T2>
inline bool sub_check_overflow(T1& num, T2 amount)
{
    static_assert(Tundra::is_integral_type<T1>::value, "T1 must be an integral \
        type.");
    static_assert(Tundra::is_integral_type<T2>::value, "T2 must be an integral \
        type.");

    return __builtin_sub_overflow(num, amount, &num);
}

/**
 * @brief Multiplies `num` by `amount`, returning true if there was an overflow.
 *
 * `num` is modified directly through pass by reference. If overflow occurs, 
 * `num`'s value is indeterminate and cannot be guaranteed to hold an expected 
 * value. 
 * 
 * `num` and `amount` must both be integral types.
 *
 * @tparam T1 Type of `num`.
 * @tparam T2 Type of `amount`.
 *
 * @param num Num to multiply .
 * @param amount Amount to multiply by.
 *
 * @return bool True if overflow has occurred.  
 */
template<typename T1, typename T2>
inline bool multiply_check_overflow(T1& num, T2 amount)
{
    static_assert(Tundra::is_integral_type<T1>::value, "T1 must be an integral \
        type.");
    static_assert(Tundra::is_integral_type<T2>::value, "T2 must be an integral \
        type.");

    return __builtin_mul_overflow(num, amount, &num);
}

} // namespace Tundra
