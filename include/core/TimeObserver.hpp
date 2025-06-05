/**
 * @file TimeObserver.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <chrono>
#include <string>


namespace Tundra
{

/**
 * @class TimeObserver
 * @brief Utility class for runtime and time-of-day tracking.
 *
 * Provides static access to:
 * - Elapsed time since program start (in fractional milliseconds)
 * - Current local time in "HH:MM:SS" format
 * - Current local date in "DD-MM-YYYY" format
 *
 * This class is non-instantiable and fully thread-safe.
 */
class TimeObserver
{
public:

    /**
     * @brief Get current local time as a string.
     *
     * Returns the system time in "HH:MM:SS" format using local time.
     *
     * @return std::string Local time as a string.
     */
    static std::string get_local_time();

    /**
     * @brief Get current local date as a string.
     *
     * Returns the system date in "DD-MM-YYYY" format using local time.
     *
     * @return std::string Local date as a string.
     */
    static std::string get_local_date();

    /**
     * @brief Get elapsed time since program start in milliseconds.
     *
     * Returns a high-resolution time duration (in fractional milliseconds) 
	 * since the program started. 
     *
     * @return double Elapsed milliseconds since program start.
     */
    static double get_elapsed();

private:

	// # MEMBERS # ------------------------------------------------------------

	static const inline std::chrono::time_point<std::chrono::steady_clock>
		s_program_start_time_point = std::chrono::steady_clock::now();
        
}; // Class TimeObserver

} // Namespace Tundra
