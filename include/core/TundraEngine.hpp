/**
 * @file TundraEngine.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Declares the TundraEngine class, the main entry point for 
 *        applications using the TUNDRA engine.
 * @version 0.1
 * @date 05-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>

#include "Renderer.hpp"


namespace Tundra {

/**
 * @class TundraEngine
 * @brief TUNDRA (Tactical Utilities for Native Design and Real-time 
 * Applications). The core of the program, runs a main simulation loop 
 * which handles internal component updating as well as proper user defined
 * code.
 * 
 * User should subclass TundraEngine and implement any user defined components,
 * then call the `start` method to begin the core simulation loop.
 */
class TundraEngine
{

public:

    TundraEngine();

    /**
     * @brief Starts the Engine's core update loop, beginning the simulation.
     */
    void start();

protected:

    // # MEMBERS # ------------------------------------------------------------

    // Screen width of the main display in pixels.
    uint16_t m_native_screen_width;

    // Screen height of the main display in pixels.
    uint16_t m_native_screen_height;

    Tundra::Renderer m_renderer;


    // # METHODS # ------------------------------------------------------------

    /**
     * @brief Flags the Engine to terminate on next frame completion.
     */
    void quit();
    

private:

    // # MEMBERS # ------------------------------------------------------------

    // Whether the Engine should keep simulating. Checked after each frame 
    // completion in the simulation loop, and will exit if this value is false.
    bool m_should_keep_simulating = false;

    // GLFW Window instance.
    GLFWwindow* m_window;

    // # METHODS # ------------------------------------------------------------

    /**
     * @brief Initializes GLFW
     */
    void _init_glfw();

    /**
     * @brief The core update loop of the Engine.
     * 
     * Updates all internal components in a closed loop, only exiting if 
     * the `m_should_keep_simulating` bool is false or a close window command
     * is registered through GLFW's events.
     */
    void _simulation_loop();

    /**
     * @brief Handles Engine cleanup after termination.
     * 
     * Called immediately after the Engine leaves the main simulation loop.
     */
    void _on_terminate();

    /**
     * @brief Intercepts and processes key press events from GLFW.
     *
     * This callback is triggered by GLFW when a keyboard key is pressed,
     * released, or held down. It allows the engine or application to handle
     * user input in a centralized manner before dispatching or consuming the
     * event.
     *
     * @param window     Pointer to the GLFW window that received the event.
     * @param key        The keyboard key that was pressed or released. 
     *                   Uses GLFW_KEY_* constants.
     * @param scancode   The system-specific scancode of the key.
     * @param action     GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
     * @param mods       Bit field describing which modifier keys were held
     *                   (GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, etc.).
     */
    static void _intercept_keypress_callback(GLFWwindow* window, int key, 
        int scancode, int action, int mods);
};

}
