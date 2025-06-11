/**
 * @file Renderer.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Declares the Renderer class which handles drawing simple primitive 
 *        shapes to the screen.
 * @version 0.1
 * @date 05-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vector>

#include "tundra/thirdparty/glad/glad.h"
#include <GLFW/glfw3.h>
#include <tundra/thirdparty/glm/glm.hpp>

#include "tundra/core/Color.hpp"


namespace Tundra
{


namespace Internal 
{
    struct Renderer; // Forward declaration for friend struct use.
}

/**
 * @brief Handles rendering primitive objects to the screen.
 * 
 * Assumes GLFW is initialized when constructed. 
 */
class Renderer
{

friend struct Tundra::Internal::Renderer;

public:

    Renderer();

    Renderer(uint16_t window_width, uint16_t window_height, 
        GLFWwindow* window);

    /**
     * @brief Submits a single triangle to the internal render buffer.
     * 
     * Each vertex is defined by a 2D position in screen space and a shared 
     * color. The triangle is not immediately rendered, it is drawn on the next 
     * internal render call.
     * 
     * @param a Position of the first vertex.
     * @param b Position of the second vertex.
     * @param c Position of the third vertex.
     * @param color RGBA color applied to all three vertices.
     */
    void draw_triangle(const glm::vec2& a, const glm::vec2& b, 
        const glm::vec2& c, const Tundra::Color& color);

    /**
     * @brief Submits a single quad to the internal render buffer.
     * 
     * Each vertex is defined by a 2D position in screen space and a shared 
     * color. The quad is not immediately rendered, it is drawn on the next 
     * internal render call.
     * 
     * @param a Position of the first vertex.
     * @param b Position of the second vertex.
     * @param c Position of the third vertex.
     * @param d Position of the fourth vertex.
     * @param color RGBA color applied to all three vertices.
     */
    void draw_quad(const glm::vec2& a, const glm::vec2& b, 
        const glm::vec2& c, const glm::vec2& d, const Tundra::Color& color);


private:

    // # STRUCTS / CLASSES # --------------------------------------------------

    /**
     * @brief Vertex data for GPU rendering including position and color.
     */
    struct Vertex 
    {
        glm::vec2 position;   // x, y
        glm::vec4 color;      // r, g, b, a
    };
    

    // # MEMBERS # ------------------------------------------------------------

    // Maximum number of vertices supported for rendering.
    static constexpr uint16_t s_MAX_VERTICES = 30000;

    // Vertex Array Object
    GLuint m_VAO;

    // Vertex Buffer Object.
    GLuint m_VBO;

    // The shader program.
    GLuint m_shader_program;

    // GPU Matrix Projection location. 
    GLint m_u_proj_loc;

    std::vector<GLfloat> m_vertices;

    // GLFW Window instance.
    GLFWwindow* m_window;


    //  # METHODS # -----------------------------------------------------------

    /**
     * @brief Underlying method for submitting triangle vertices to the buffer.
     * 
     * @param a Position of the first vertex.
     * @param b Position of the second vertex.
     * @param c Position of the third vertex.
     * @param color RGBA color applied to all three vertices.
     */
    void internal_draw_triangle(const glm::vec2& a, const glm::vec2& b, 
        const glm::vec2& c, const Tundra::Color& color);

    /**
     * @brief Initializes OpenGL components such as the shaders and VAO/VBO.
     * 
     * @param window_width Width of the window to create.
     * @param window_height Height of the window to create.
     */
    void init_OpenGL_components(uint16_t window_width, 
        uint16_t window_height);

    /**
     * @brief Creates and binds the components associated with vertexes, such 
     * as the VAO, VBO and Vertex Attributes.
     */
    void create_vertex_components();

    /**
     * @brief Creates the shader program for OpenGL.
     */
    void create_shader_program();

    /**
     * @brief Compiles a single shader, checking for any compilation errors.
     * 
     * @param shader_type Type of shader.
     * @param shader_src Source code to compile.
     * @return GLuint 
     */
    GLuint compile_shader(GLenum shader_type, const char* shader_src);
    
}; // Class Renderer

} // Namespace Tundra
