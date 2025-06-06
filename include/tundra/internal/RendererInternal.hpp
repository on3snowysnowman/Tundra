/**
 * @file Renderer.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/core/Renderer.hpp"


namespace Tundra
{

namespace Internal
{

/**
 * @struct Renderer
 * @brief Internal-only interface for managing Renderer clear and present screen
 * calls.
 * 
 * This internal struct provides the necessary function class for clearing and
 * presenting the buffers for rendering. It is intended for use by the core 
 * engine systems and should not be accessed by users.
 */
struct Renderer
{
    /**
     * @brief Clears internal screen buffer to ready for the next frame.
     * 
     * This is called by the TundraEngine, users should avoid this method.
     */
    static void clear_screen();

    /**
     * @brief Presents the screen buffer to the screen.
     * 
     * This is called by the TundraEngine, users should avoid this method.
     */
    static void present_screen(Tundra::Renderer& renderer);

    /**
     * @brief Cleans up internal components before Renderer deletion.
     */
    static void cleanup(Tundra::Renderer& renderer);

};

} // Namespace Internal

} // Namespace Tundra

