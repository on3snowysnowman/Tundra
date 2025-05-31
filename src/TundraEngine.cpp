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
#include "KeyCodes.hpp"
#include "KeyCodeConversions.hpp"


// GLFW Keycodes to TUNDRA keycodes converstion table when shift is not pressed.
// static const std::unordered_map<int, Tundra::KEYCODE> UNSHIFTED_KEY_CONVERSION 
// {
//     // Letters
//     {GLFW_KEY_A, Tundra::TUNDRA_a}, {GLFW_KEY_B, Tundra::TUNDRA_b},
//     {GLFW_KEY_C, Tundra::TUNDRA_c}, {GLFW_KEY_D, Tundra::TUNDRA_d},
//     {GLFW_KEY_E, Tundra::TUNDRA_e}, {GLFW_KEY_F, Tundra::TUNDRA_f},
//     {GLFW_KEY_G, Tundra::TUNDRA_g}, {GLFW_KEY_H, Tundra::TUNDRA_h},
//     {GLFW_KEY_I, Tundra::TUNDRA_i}, {GLFW_KEY_J, Tundra::TUNDRA_j},
//     {GLFW_KEY_K, Tundra::TUNDRA_k}, {GLFW_KEY_L, Tundra::TUNDRA_l},
//     {GLFW_KEY_M, Tundra::TUNDRA_m}, {GLFW_KEY_N, Tundra::TUNDRA_n},
//     {GLFW_KEY_O, Tundra::TUNDRA_o}, {GLFW_KEY_P, Tundra::TUNDRA_p},
//     {GLFW_KEY_Q, Tundra::TUNDRA_q}, {GLFW_KEY_R, Tundra::TUNDRA_r},
//     {GLFW_KEY_S, Tundra::TUNDRA_s}, {GLFW_KEY_T, Tundra::TUNDRA_t},
//     {GLFW_KEY_U, Tundra::TUNDRA_u}, {GLFW_KEY_V, Tundra::TUNDRA_v},
//     {GLFW_KEY_W, Tundra::TUNDRA_w}, {GLFW_KEY_X, Tundra::TUNDRA_x},
//     {GLFW_KEY_Y, Tundra::TUNDRA_y}, {GLFW_KEY_Z, Tundra::TUNDRA_z},

//     // Digits
//     {GLFW_KEY_0, Tundra::TUNDRA_0}, {GLFW_KEY_1, Tundra::TUNDRA_1},
//     {GLFW_KEY_2, Tundra::TUNDRA_2}, {GLFW_KEY_3, Tundra::TUNDRA_3},
//     {GLFW_KEY_4, Tundra::TUNDRA_4}, {GLFW_KEY_5, Tundra::TUNDRA_5},
//     {GLFW_KEY_6, Tundra::TUNDRA_6}, {GLFW_KEY_7, Tundra::TUNDRA_7},
//     {GLFW_KEY_8, Tundra::TUNDRA_8}, {GLFW_KEY_9, Tundra::TUNDRA_9},

//     // Symbols (unshifted)
//     {GLFW_KEY_SPACE, Tundra::TUNDRA_SPACE}, 
//         {GLFW_KEY_APOSTROPHE, Tundra::TUNDRA_APOSTROPHE},
//     {GLFW_KEY_COMMA, Tundra::TUNDRA_COMMA},
//          {GLFW_KEY_MINUS, Tundra::TUNDRA_MINUS},
//     {GLFW_KEY_PERIOD, Tundra::TUNDRA_PERIOD}, 
//         {GLFW_KEY_SLASH, Tundra::TUNDRA_SLASH},
//     {GLFW_KEY_SEMICOLON, Tundra::TUNDRA_SEMICOLON}, 
//         {GLFW_KEY_EQUAL, Tundra::TUNDRA_EQUAL},
//     {GLFW_KEY_LEFT_BRACKET, Tundra::TUNDRA_LEFT_BRACKET},
//     {GLFW_KEY_BACKSLASH, Tundra::TUNDRA_BACKSLASH},
//     {GLFW_KEY_RIGHT_BRACKET, Tundra::TUNDRA_RIGHT_BRACKET},
//     {GLFW_KEY_GRAVE_ACCENT, Tundra::TUNDRA_GRAVE},

//     // Control characters
//     {GLFW_KEY_ENTER, Tundra::TUNDRA_RETURN}, {GLFW_KEY_TAB, Tundra::TUNDRA_TAB},
//     {GLFW_KEY_BACKSPACE, Tundra::TUNDRA_BACKSPACE}, 
//         {GLFW_KEY_ESCAPE, Tundra::TUNDRA_ESCAPE},
//     {GLFW_KEY_DELETE, Tundra::TUNDRA_DELETE}
// };

// // GLFW Keycodes to TUNDRA keycodes converstion table when shift is pressed.
// static const std::unordered_map<int, Tundra::KEYCODE> SHIFTED_KEY_CONVERSION
// {
//     // Letters (uppercase)
//     {GLFW_KEY_A, Tundra::TUNDRA_A}, {GLFW_KEY_B, Tundra::TUNDRA_B},
//     {GLFW_KEY_C, Tundra::TUNDRA_C}, {GLFW_KEY_D, Tundra::TUNDRA_D},
//     {GLFW_KEY_E, Tundra::TUNDRA_E}, {GLFW_KEY_F, Tundra::TUNDRA_F},
//     {GLFW_KEY_G, Tundra::TUNDRA_G}, {GLFW_KEY_H, Tundra::TUNDRA_H},
//     {GLFW_KEY_I, Tundra::TUNDRA_I}, {GLFW_KEY_J, Tundra::TUNDRA_J},
//     {GLFW_KEY_K, Tundra::TUNDRA_K}, {GLFW_KEY_L, Tundra::TUNDRA_L},
//     {GLFW_KEY_M, Tundra::TUNDRA_M}, {GLFW_KEY_N, Tundra::TUNDRA_N},
//     {GLFW_KEY_O, Tundra::TUNDRA_O}, {GLFW_KEY_P, Tundra::TUNDRA_P},
//     {GLFW_KEY_Q, Tundra::TUNDRA_Q}, {GLFW_KEY_R, Tundra::TUNDRA_R},
//     {GLFW_KEY_S, Tundra::TUNDRA_S}, {GLFW_KEY_T, Tundra::TUNDRA_T},
//     {GLFW_KEY_U, Tundra::TUNDRA_U}, {GLFW_KEY_V, Tundra::TUNDRA_V},
//     {GLFW_KEY_W, Tundra::TUNDRA_W}, {GLFW_KEY_X, Tundra::TUNDRA_X},
//     {GLFW_KEY_Y, Tundra::TUNDRA_Y}, {GLFW_KEY_Z, Tundra::TUNDRA_Z},

//     // Digits (shifted to symbols)
//     {GLFW_KEY_1, Tundra::TUNDRA_EXCLAMATION}, {GLFW_KEY_2, Tundra::TUNDRA_AT},
//     {GLFW_KEY_3, Tundra::TUNDRA_HASH}, {GLFW_KEY_4, Tundra::TUNDRA_DOLLAR},
//     {GLFW_KEY_5, Tundra::TUNDRA_PERCENT}, {GLFW_KEY_6, Tundra::TUNDRA_CARET},
//     {GLFW_KEY_7, Tundra::TUNDRA_AMPERSAND}, 
//         {GLFW_KEY_8, Tundra::TUNDRA_ASTERISK},
//     {GLFW_KEY_9, Tundra::TUNDRA_LEFT_PAREN}, 
//         {GLFW_KEY_0, Tundra::TUNDRA_RIGHT_PAREN},

//     // Symbols (shifted)
//     {GLFW_KEY_SPACE, Tundra::TUNDRA_SPACE}, 
//         {GLFW_KEY_APOSTROPHE, Tundra::TUNDRA_QUOTE},
//     {GLFW_KEY_COMMA, Tundra::TUNDRA_LESS_THAN}, 
//         {GLFW_KEY_MINUS, Tundra::TUNDRA_UNDERSCORE},
//     {GLFW_KEY_PERIOD, Tundra::TUNDRA_GREATER_THAN}, 
//         {GLFW_KEY_SLASH, Tundra::TUNDRA_QUESTION},
//     {GLFW_KEY_SEMICOLON, Tundra::TUNDRA_COLON}, 
//         {GLFW_KEY_EQUAL, Tundra::TUNDRA_PLUS},
//     {GLFW_KEY_LEFT_BRACKET, Tundra::TUNDRA_LEFT_BRACE},
//     {GLFW_KEY_BACKSLASH, Tundra::TUNDRA_PIPE},
//     {GLFW_KEY_RIGHT_BRACKET, Tundra::TUNDRA_RIGHT_BRACE},
//     {GLFW_KEY_GRAVE_ACCENT, Tundra::TUNDRA_TILDE},

//     // Control characters (same as unshifted)
//     {GLFW_KEY_ENTER, Tundra::TUNDRA_RETURN}, {GLFW_KEY_TAB, Tundra::TUNDRA_TAB},
//     {GLFW_KEY_BACKSPACE, Tundra::TUNDRA_BACKSPACE}, 
//         {GLFW_KEY_ESCAPE, Tundra::TUNDRA_ESCAPE},
//     {GLFW_KEY_DELETE, Tundra::TUNDRA_DELETE}
// };



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
    
    // Run simulation loop until the Engine stops.
    _simulation_loop();

    // End of simultion,
    _on_terminate();
}


// Protected

void Tundra::TundraEngine::_quit()
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

    glfwSetKeyCallback(m_window, TundraEngine::_intercept_keypress_callback);
}

void Tundra::TundraEngine::_simulation_loop()
{
    while(m_should_keep_simulating && !glfwWindowShouldClose(m_window))
    {
        InputManager::_reset_raw_pressed_keys();

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

void Tundra::TundraEngine::_intercept_keypress_callback(GLFWwindow* window, 
    int key, int scancode, int action, int mods)
{
    // Invalid key.
    // if(SHIFTED_KEY_CONVERSION.find(key) == SHIFTED_KEY_CONVERSION.end()) return;

    Tundra::KEYCODE converted_key;

    // Shift is pressed.
    if(mods & GLFW_MOD_SHIFT)
    {
        converted_key = Tundra::Internal::SHIFTED_KEY_CONVERSION.at(key);
    }

    else converted_key = Tundra::Internal::UNSHIFTED_KEY_CONVERSION.at(key);

    if(converted_key == TUNDRA_INVALID_KEYCODE) return;

    switch(action)
    {
        case GLFW_PRESS:

            InputManager::_flag_key_pressed(converted_key);
            break;

        case GLFW_RELEASE:

            InputManager::_flag_key_released(converted_key);
            break;
    }
}
