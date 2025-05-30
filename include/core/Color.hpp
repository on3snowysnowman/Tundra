/**
 * @file Color.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-28-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>


namespace Tundra
{

/**
 * @brief Standard color container with r, g, b, a values. Range : [0 - 255].
 */
struct Color
{
    uint8_t r{}, g{}, b{}, a{};
};

}
