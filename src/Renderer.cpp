/**
 * @file Renderer.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/core/Renderer.hpp"
#include "tundra/internal/RendererInternal.hpp"

#include <iostream>

#include <tundra/thirdparty/glm/gtc/matrix_transform.hpp>
#include <tundra/thirdparty/glm/gtc/type_ptr.hpp>


const char* vertex_shader_src = R"glsl(
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_Projection;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_Projection * vec4(a_Position, 0.0, 1.0);
}
)glsl";

const char* fragment_shader_src = R"glsl(
#version 330 core

in vec4 v_Color;
out vec4 FragColor;

void main()
{
    FragColor = v_Color;
}
)glsl";



// Constructors / Deconstructor

Tundra::Renderer::Renderer()
{
    m_VAO = 0;
    m_VBO = 0;
    m_window = nullptr;
}

Tundra::Renderer::Renderer(uint16_t window_width, uint16_t window_height, 
    GLFWwindow* window)
{
    m_window = window;

    init_OpenGL_components(window_width, window_height);
}


// Public

// # INTERNAL # ----------------------------------------------------------------

void Tundra::Internal::Renderer::clear_screen() 
{
    // Specifies background color.
    glClearColor(0.05f, 0.05f, 0.07f, 1);

    // Clears the backbuffer with the color at the color bit buffer.
    glClear(GL_COLOR_BUFFER_BIT);
}

void Tundra::Internal::Renderer::present_screen(Tundra::Renderer& renderer) 
{
    // Upload vertex data to the GPU.
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
        renderer.m_vertices.size() * sizeof(GLfloat),
        renderer.m_vertices.data());

    // Draw the buffered vertices to the screen, which are all in increments 
    // of Triangles represented by 6 floats.
    glDrawArrays(GL_TRIANGLES, 0, renderer.m_vertices.size() / 6);

    // Swap the buffers, presenting the buffered screen. 
    glfwSwapBuffers(renderer.m_window);
}

void Tundra::Internal::Renderer::cleanup(Tundra::Renderer& renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &renderer.m_VAO);
    glDeleteBuffers(1, &renderer.m_VBO);
    glDeleteProgram(renderer.m_shader_program);
    glfwDestroyWindow(renderer.m_window);
}

// # PUBLIC API # --------------------------------------------------------------

void Tundra::Renderer::draw_triangle(const glm::vec2& a, const glm::vec2& b, 
    const glm::vec2& c, const Tundra::Color& color)
{
    internal_draw_triangle(a, b, c, color);
}

void Tundra::Renderer::draw_quad(const glm::vec2& a, const glm::vec2& b, 
    const glm::vec2& c, const glm::vec2& d, const Tundra::Color& color)
{
    internal_draw_triangle(a, b, c, color);
    internal_draw_triangle(c, b, d, color);
}


// Private

void Tundra::Renderer::internal_draw_triangle(const glm::vec2& a, const glm::vec2& b, 
    const glm::vec2& c, const Tundra::Color& color)
{
    // Convert 0-255 range based Color into a 0-1.0 range for OpenGl.
    glm::vec4 float_converted_color = glm::vec4(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );

    // Create complete vertex containers from the positions and color.
    Vertex v1 = { a, float_converted_color };
    Vertex v2 = { b, float_converted_color };
    Vertex v3 = { c, float_converted_color };

    // Append vertexs to vertex buffer.
    m_vertices.insert(m_vertices.end(), {
        v1.position.x, v1.position.y,
        v1.color.r, v1.color.g, v1.color.b, v1.color.a,

        v2.position.x, v2.position.y,
        v2.color.r, v2.color.g, v2.color.b, v2.color.a,

        v3.position.x, v3.position.y,
        v3.color.r, v3.color.g, v3.color.b, v3.color.a
    });
}

void Tundra::Renderer::init_OpenGL_components(uint16_t window_width, 
    uint16_t window_height)
{
    create_vertex_components();
    create_shader_program();

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(window_width),
        static_cast<float>(window_height), 0.0f,
        -1.0f, 1.0f);

    // Find the location of the projection inside the shader.
    m_u_proj_loc = glGetUniformLocation(m_shader_program, 
        "u_Projection");
    
    // Hand the projection calculated by glm to the shader.
    glUniformMatrix4fv(m_u_proj_loc, 1, GL_FALSE, 
        glm::value_ptr(projection));
    
    // Tell OpenGl the viewport, or area of the window we want it to render in.
    glViewport(0, 0, window_width, window_height);
}

void Tundra::Renderer::create_vertex_components()
{
    // Geenrate and bind Vertex Array.
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Generate and bind Vertex Buffers as well as allocated GPU memory.
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,s_MAX_VERTICES * sizeof(Vertex), nullptr, 
        GL_DYNAMIC_DRAW);
        
    // Assign position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (void*)0);

    // Assign color attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (void*)offsetof(Vertex, Vertex::color));
}

void Tundra::Renderer::create_shader_program()
{
    m_shader_program = glCreateProgram();

    // Compile both shaders.
    GLuint m_vertex_shader_ID = compile_shader(GL_VERTEX_SHADER, 
        vertex_shader_src);
    GLuint m_fragment_shader_ID = compile_shader(GL_FRAGMENT_SHADER, 
        fragment_shader_src);

    // Compile and attach shaders.
    glAttachShader(m_shader_program, m_vertex_shader_ID);
    glAttachShader(m_shader_program, m_fragment_shader_ID);

    glLinkProgram(m_shader_program);

    // Check for linking errors
    GLint success_flag;
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, &success_flag);
    
    if (!success_flag) {
        
        char info_log[512];
        glGetProgramInfoLog(m_shader_program, 512, nullptr, info_log);
        std::cerr << "Shader linking failed:\n" << info_log << '\n';
        exit(-1);
    }

    // Delete shaders.
    glDeleteShader(m_vertex_shader_ID);
    glDeleteShader(m_fragment_shader_ID);

    glUseProgram(m_shader_program);
}

GLuint Tundra::Renderer::compile_shader(GLenum shader_type, 
    const char* shader_src)
{
    // Get the ID of the created shader.
    GLuint shader_ID = glCreateShader(shader_type);
    
    // Hand over shader source code.
    glShaderSource(shader_ID, 1, &shader_src, NULL);

    glCompileShader(shader_ID);

    // Check for compilation erros.
    GLint success_flag;
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &success_flag);
    if(!success_flag)
    {
        char info_log[512];
        glGetShaderInfoLog(shader_ID, 512, nullptr, info_log);
        std::cerr << "[ERR] Shader compilation failed:\n" << info_log << '\n';
        exit(-1);
    }

    return shader_ID;
}
