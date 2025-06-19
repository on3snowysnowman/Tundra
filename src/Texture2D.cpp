/**
 * @file Texture2D.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-11-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/core/Texture2D.hpp"

#include <iostream>


Tundra::Texture2D Tundra::create_texture(const Tundra::PNG_Data& png_data)
{   
    GLuint new_texture_id;

    // Generate an OpenGL texture.
    glGenTextures(1, &new_texture_id);
    glBindTexture(GL_TEXTURE_2D, new_texture_id);
    
    // Flag OpenGL there is no padding in the pixel data.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Internal GPU format flag that will be handed to OpenGl. Value is 
    // determined by the PNG_Data's pixel format.
    GLint targ_gl_texture_internal_format = GL_RGBA8;

    // Pixel format flag that will be handed to OpenGL. Value is determined by
    // the PNG_Data's pixel format.
    GLint targ_gl_texture_pixel_format = GL_RGBA;

    // Channel type flag that will be handed to OpenGL. 
    GLint targ_gl_texture_channel_type = GL_UNSIGNED_BYTE;

    switch(png_data.pixel_format)
    {
        case Tundra::PIXEL_FORMAT::GRAYSCALE:

            // 1 Channel, 8 bit depth.
            targ_gl_texture_internal_format = GL_R8;
            targ_gl_texture_pixel_format = GL_RED;

            break;

        case Tundra::PIXEL_FORMAT::RGB:

            // 3 Channels, 8 bit depth.
            targ_gl_texture_internal_format = GL_RGB8;
            targ_gl_texture_pixel_format = GL_RGB;

            break;

        case Tundra::PIXEL_FORMAT::GRAYSCALE_ALPHA:

            // 2 Channels, 8 bit depth.
            targ_gl_texture_internal_format = GL_RG8;
            targ_gl_texture_pixel_format = GL_RG;
    
            break;

        case Tundra::PIXEL_FORMAT::RGBA:

            // 4 Channels, 8 bit depth.
            targ_gl_texture_internal_format = GL_RGBA8;
            targ_gl_texture_pixel_format = GL_RGBA;

            break;

        default:

            // @todo Replace this with a crash handler call, and use a TundraEngine
            // callback to softly crash.
            std::cout << "[ERR] create_texture() : Passed PNG Data has an "
                "invalid pixel format.\n";
            exit(-1); 
            break;
    }

    // Pass texture data to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, targ_gl_texture_internal_format, 
        png_data.image_width, png_data.image_height, 0, 
        targ_gl_texture_pixel_format, targ_gl_texture_channel_type, 
        png_data.pixel_data.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    // Create and return texture object.
    return {uint16_t(png_data.image_width), uint16_t(png_data.image_height), 
        new_texture_id};
}
