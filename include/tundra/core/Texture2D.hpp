/**
 * @file Texture2D.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-11-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>

#include <GLFW/glfw3.h>

#include <tundra/core/PNGLoader.hpp>


namespace Tundra
{

/**
 * @brief Represents a 2D texture used for rendering.
 * 
 * Stores essential data such as dimensions and the OpenGL texture ID.
 * 
 * Users should not create a Texture instance manually, use the TextureCreator
 */
struct Texture2D
{
	// Width of the image in pixels.
	const uint16_t m_width_pixels;

	// Height of the texture in pixels.
	const uint16_t m_height_pixels;

	// ID of the OpenGL handle for this texture.
	const GLuint m_id; 
	
}; // Struct Texture


/**
 * @brief Creates a 2D Texture from PNG image data.
 * 
 * @param png_data Data to create texture from.
 * @return Texture2D Newly created texture.
 */
Texture2D create_texture(const Tundra::PNG_Data& png_data);

} // Namespace Tundra
