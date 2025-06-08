/**
 * @file PNGLoader.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <tundra/core/PNGLoader.hpp>

#include <iostream>

#include <zlib.h>

// Chunk signatures as a decimal representation of 4 byte integers rather 
// than the 4 bytes of ASCII characters.

#define DECIMAL_IHDR_SIGNATURE 1229472850
#define DECIMAL_PLTE_SIGNATURE 1347179589
#define DECIMAL_IDAT_SIGNATURE 1229209940
#define DECIMAL_IEND_SIGNATURE 1229278788


// Constructors / Deconstructor

Tundra::PNGLoader::PNGLoader() {}


// Public

Tundra::PNG_Data Tundra::PNGLoader::load_png(const char* png_path)
{
    if(!m_bin_parser.open_file(png_path))
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::load_png() : Failed to open PNG file.\n";
        exit(-1);
    }

    // If the signature of the open file does not match the PNG signature.
    if(!verify_PNG_signature())
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::load_png() : Open file's signature does not"
        " match the PNG signature.\n";
        exit(-1);
    }

    read_IHDR_chunk();

    // If this image is palette indexed, handle it.
    if(m_is_indexed) handle_indexed_image();

    // Handle non palette indexed image.
    else handle_un_indexed_image();

    m_bin_parser.close_file();

    // Copy the active data that has been built from handling the PNG. This 
    // copied object will be returned to the caller. The reason the active data
    // isn't directly returned as a copy through a return statement, is the 
    // active data pixels pointer needs to be set to nullptr AFTER a copy has 
    // been made for the caller, so we copy the object first, then set the 
    // original active data pointer to nullptr to leave in a valid state.
    PNG_Data copied_data = PNG_Data(m_active_data);

    m_active_data.pixels = nullptr;

    // Leave the other values of the active data as residual values, since we 
    // know that they will be replaced the next time this function is called and
    // don't really care if there is junk integers in there.

    return copied_data;
}


// Private

void Tundra::PNGLoader::read_IHDR_chunk()
{
    constexpr uint32_t STANDARD_IHDR_CHUNK_LENGTH = 13;

    uint32_t chunk_length = m_bin_parser.read_four_bytes();
    uint32_t chunk_type = m_bin_parser.read_four_bytes();

    // Chunk type does not match the IHDR signature.
    if(chunk_type != DECIMAL_IHDR_SIGNATURE || 
        chunk_length != STANDARD_IHDR_CHUNK_LENGTH)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Chunk expected to be IHDR"
            " had invalid size or signature.\n";
        exit(-1);
    }

    // Fetch image dimensions.
    m_active_data.image_width = m_bin_parser.read_four_bytes();
    m_active_data.image_height = m_bin_parser.read_four_bytes();

    // If the next byte, which is bit depth per channel of pixel is not 8.
    if(m_bin_parser.read_byte() != 8)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Bit depth per channel is"
        " not 8 bits. Only 8 bits channels are supported.\n";
        exit(-1);
    }

    uint8_t color_type = m_bin_parser.read_byte();

    // Reset indexed flag.
    m_is_indexed = false;

    // Handle color type.
    switch(color_type)
    {
        // GRAYSCALE
        case 0:

            m_active_data.pixel_format = Tundra::PIXEL_FORMAT::GRAYSCALE;
            break;

        // RGB
        case 2:

            m_active_data.pixel_format = Tundra::PIXEL_FORMAT::RGB;
            break;

        // Palette Indexed (Final result will be RGB)
        case 3:

            m_active_data.pixel_format = Tundra::PIXEL_FORMAT::RGB;
            m_is_indexed = true;
            break;

        // GRAYSCALE + ALPHA
        case 4:

            m_active_data.pixel_format = Tundra::PIXEL_FORMAT::GRAYSCALE_ALPHA;
            break;

        // RGBA
        case 6:

            m_active_data.pixel_format = Tundra::PIXEL_FORMAT::RGBA;
            break;

        // Invalid color index.
        default:

            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cerr << "PNGLoader::read_IHDR_chunk() : Invalid color index."
                << '\n';
            exit(-1);
    }

    // If the compression byte is not 0.
    if(m_bin_parser.read_byte() != 0)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Compression byte is not 0."
            << '\n';
        exit(-1);
    }

    // If the compression byte is not 0.
    if(m_bin_parser.read_byte() != 0)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Filter byte is not 0."
            << '\n';
        exit(-1);
    }

    // If the interlace byte is not 0.
    if(m_bin_parser.read_byte() != 0)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Interlace byte is not 0"
            << '\n';
        exit(-1);
    }

    uint32_t crc = m_bin_parser.read_four_bytes();


    // Valid IHDR has been read.
}

void Tundra::PNGLoader::handle_indexed_image()
{
    
}

void Tundra::PNGLoader::handle_un_indexed_image() {}

bool Tundra::PNGLoader::verify_PNG_signature()
{
    constexpr uint8_t PNG_SIGNATURE[] = {137, 80, 78, 71, 13, 10, 26, 10};
    constexpr uint8_t PNG_SIGNATURE_SIZE = 8;

    // Iterate through the size of the signature in bytes of the open PNG file,
    // ensuring each byte matches the signature.
    for(uint8_t i = 0; i < PNG_SIGNATURE_SIZE; ++i)
    {
        // If this byte does not match the corresponding byte in the signature.
        if(m_bin_parser.read_byte() != PNG_SIGNATURE[i]) return false;
    }

    // All bytes have been read and verified.
    return true;
}

// bool Tundra::PNGLoader::verify_CRC(uint32_t crc, uint32_t chunk_length, 
//     uint32_t chunk_type) const
// {
//     uLong 
// }

uint32_t Tundra::PNGLoader::find_PLTE_chunk()
{
    uint32_t chunk_size = m_bin_parser.read_four_bytes();
    uint32_t chunk_type = m_bin_parser.read_four_bytes();

    while(chunk_type != DECIMAL_PLTE_SIGNATURE)
    {
        if(chunk_type == DECIMAL_IEND_SIGNATURE)
        {
            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cerr << "PNGLoader::find_PLTE_chunk() : Reached end of PNG "
                "data and did not find PLTE chunk on palette indexed image.\n";
            exit(-1);
        }

        // Skip over chunk data and crc.
        for(uint32_t i = 0; i < chunk_size + 4; ++i)
        {
            m_bin_parser.read_byte();
        }
    }
}

