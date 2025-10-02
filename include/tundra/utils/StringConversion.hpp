/**
 * @file StringConversion.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for converting numbers/floats to Strings and Strings to 
 *    numbers/floats.
 * @version 0.1
 * @date 09-29-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/utils/containers/String.hpp"


namespace Tundra
{


// int string_to_int(const Tundra::Str::String *str)

/**
 * @brief Converts an integer into its readable form. 
 * 
 * @param num Num to convert.
 *
 * @return Tundra::Str::String Readable int. 
 */
Tundra::Str::String<> int_to_string(int num);

} // namespace Tundra
