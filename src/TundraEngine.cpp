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

#include "TundraEngine.hpp"

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "InputManager.hpp"

// Constructors / Deconstructor

Tundra::TundraEngine::TundraEngine() 
{
    _init_glfw();

    m_renderer = Tundra::Renderer(800, 800, m_window);
}


// Public

void Tundra::TundraEngine::start() 
{
    m_should_keep_simulating = true;
    _simulation_loop();
}


// Protected

void Tundra::TundraEngine::_quit()
{
    m_should_keep_simulating = false;
}

void Tundra::TundraEngine::print_hello()
{
    std::cout << "Hello!\n";
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
        "OpenGL Experiments", NULL, NULL);

    if(m_window == NULL)
    {
        std::cout << "[ERR] Failed to create GLFW window.\n";
        exit(-1);
    }

    // Tell glfw that this window is part of the main context.
    glfwMakeContextCurrent(m_window);

    // Tell glad to load the configurations for OpenGl.
    gladLoadGL();
}

void Tundra::TundraEngine::_simulation_loop()
{
    while(m_should_keep_simulating && !glfwWindowShouldClose(m_window))
    {
        m_renderer._clear_screen();

        m_renderer.draw_triangle({100, 100}, {120, 200}, {80, 200}, 
            {0, 60, 220, 255});

        m_renderer.draw_quad({200, 100}, {720, 100}, {200, 300}, {400, 300},
            {0, 30, 175, 255});

        m_renderer._present_screen();
        
        glfwPollEvents();
    }
}

void Tundra::TundraEngine::_on_terminate()
{
    m_renderer.cleanup();
    glfwTerminate();
}
