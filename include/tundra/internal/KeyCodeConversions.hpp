/**
 * @file KeyCodeConversions.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <array>

#include "tundra/core/KeyCodes.hpp"


namespace Tundra::Internal 
{

extern const std::array<Tundra::KEYCODE, 349> UNSHIFTED_KEY_CONVERSION;
extern const std::array<Tundra::KEYCODE, 349> SHIFTED_KEY_CONVERSION;

}
