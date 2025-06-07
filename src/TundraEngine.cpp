/**
 * @file TundraEngine.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Implementation for the TundraEngine class.
 * @version 0.1
 * @date 05-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/core/TundraEngine.hpp"

#include <iostream>

#include "tundra/thirdparty/glad/glad.h"
#include "GLFW/glfw3.h"

#include "tundra/core/InputManager.hpp"
#include "tundra/core/KeyCodes.hpp"
#include "tundra/core/TimeObserver.hpp"
#include "tundra/internal/KeyCodeConversions.hpp"
#include "tundra/internal/InputManagerInternal.hpp"
#include "tundra/internal/RendererInternal.hpp"


// Constructors / Deconstructor

Tundra::TundraEngine::TundraEngine() 
{
    _init_glfw();

    m_renderer = Tundra::Renderer(m_window_width, m_window_height, m_window);

    InputManager::register_callback<TundraEngine>(Tundra::KEYCODE::ESCAPE,
        &TundraEngine::quit, this);
    InputManager::register_callback<TundraEngine>(Tundra::KEYCODE::LOWER_A,
        &TundraEngine::move_triangle_left, this);
    InputManager::register_callback<TundraEngine>(Tundra::KEYCODE::LOWER_W,
        &TundraEngine::move_triangle_up, this);
    InputManager::register_callback<TundraEngine>(Tundra::KEYCODE::LOWER_D,
        &TundraEngine::move_triangle_right, this);
    InputManager::register_callback<TundraEngine>(Tundra::KEYCODE::LOWER_S,
        &TundraEngine::move_triangle_down, this);
}


// Public

void Tundra::TundraEngine::start() 
{
    m_should_keep_simulating = true;
    
    // Run simulation loop until the Engine stops.
    _simulation_loop();

    // End of simultion,
    _on_terminate();
}


// Protected

void Tundra::TundraEngine::quit()
{
    m_should_keep_simulating = false;
}


// Private

void Tundra::TundraEngine::_init_glfw()
{
    if(glfwInit() == GLFW_FALSE)
    {
        std::cout << "[ERR] GLFW failed to initialize.\n";
        exit(-1);
    }

    // Tell glfw what major version of OpenGL is being used.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    // Tell glfw what minor version of OpenGl is being used.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Tell glfw what "kind" of package is being used, whether that be the core
    // package (modern), or the compatibility package (modern + outdated).
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(800, 800, 
        "Tundra", NULL, NULL);

    if(m_window == NULL)
    {
        std::cout << "[ERR] Failed to create GLFW window.\n";
        exit(-1);
    }

    // Tell glfw that this window is part of the main context.
    glfwMakeContextCurrent(m_window);

    // Tell glad to load the configurations for OpenGl.
    gladLoadGL();

    glfwSetKeyCallback(m_window, TundraEngine::_intercept_keypress_callback);
}

void Tundra::TundraEngine::_simulation_loop()
{
    while(m_should_keep_simulating && !glfwWindowShouldClose(m_window))
    {
        // Clear screen.
        Tundra::Internal::Renderer::clear_screen(m_renderer);

        Tundra::Internal::InputManager::handle_pressed_keys();

        for(int i = 0; i < 10000; ++i)
        {
            m_renderer.draw_triangle({100 + vertex_x_offset, 100 + vertex_y_offset}, 
                {120 + vertex_x_offset, 200 + vertex_y_offset}, 
                {80 + vertex_x_offset, 200 + vertex_y_offset}, 
                {0, 60, 220, 255});
        }
        // m_renderer.draw_quad({200, 100}, {720, 100}, {200, 300}, {400, 300},
        //     {0, 30, 175, 255});

        // Present screen.
        Tundra::Internal::Renderer::present_screen(m_renderer);

        // Clear InputManager's raw keys in preperation for the next key events.
        Tundra::Internal::InputManager::reset_raw_pressed_keys();

        // Poll GLFW events.
        glfwPollEvents();
    }
}

void Tundra::TundraEngine::_on_terminate()
{
    Tundra::Internal::Renderer::cleanup(m_renderer);
    glfwTerminate();
}

void Tundra::TundraEngine::_intercept_keypress_callback(GLFWwindow* window, 
    int key, int scancode, int action, int mods)
{
    Tundra::KEYCODE converted_key;

    // We need to convert the GLFW key into Tundra::KEYCODE (ASCII based). Use
    // the lookup table(s), depending on if shift is pressed.

    // Shift is pressed.
    if(mods & GLFW_MOD_SHIFT)
    {
        converted_key = Tundra::Internal::SHIFTED_KEY_CONVERSION.at(key);
    }

    else converted_key = Tundra::Internal::UNSHIFTED_KEY_CONVERSION.at(key);

    // The key that was converted from GLFW is not in supported range:
    // (ASCII range [8, 127]).
    if(converted_key == Tundra::KEYCODE::INVALID_KEYCODE) return;

    switch(action)
    {
        case GLFW_PRESS:

            Tundra::Internal::InputManager::flag_key_pressed(converted_key);
            break;

        case GLFW_RELEASE:

            Tundra::Internal::InputManager::flag_key_released(converted_key);
            break;
    }
}

void Tundra::TundraEngine::move_triangle_left() 
{
    vertex_x_offset -= (vertex_x_offset > 0);
}

void Tundra::TundraEngine::move_triangle_right() 
{
    vertex_x_offset += (vertex_x_offset < (m_window_width - 50));
}

void Tundra::TundraEngine::move_triangle_up() 
{
    vertex_y_offset -= (vertex_y_offset > 0);
}

void Tundra::TundraEngine::move_triangle_down() 
{
    vertex_y_offset += (vertex_y_offset < m_window_height - 50);
}
