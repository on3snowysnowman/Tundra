/**
 * @file TimeObserver.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-31-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "TimeObserver.hpp"


// Public

std::string Tundra::TimeObserver::get_local_time() 
{
    // Get current system time
    std::time_t t = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    // Convert to local time
    std::tm local_tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&local_tm, &t);  // Windows
#else
    localtime_r(&t, &local_tm);  // POSIX
#endif

    // Format time as HH:MM:SS
    char buffer[16];  // Enough for "HH:MM:SS\0"
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_tm);

    return std::string(buffer);
}

std::string Tundra::TimeObserver::get_local_date()
{
     // Get current system time
    std::time_t t = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    // Convert to local time
    std::tm local_tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&local_tm, &t);  // Windows safe version
#else
    localtime_r(&t, &local_tm);  // POSIX safe version
#endif

    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", &local_tm);
    return std::string(buffer); 
}

double Tundra::TimeObserver::get_elapsed() 
{
    return static_cast<std::chrono::duration<double, std::milli>>( 
        std::chrono::steady_clock::now() - s_program_start_time_point).count();
}
