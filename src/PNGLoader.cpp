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

    PNG_Data png_data;

    // This is a pointer to local stack allocated memory! This pointer is used
    // by the class during the PNG loading to populate the data container, and 
    // at the end of this function, the pointer is safely assigned to nullptr 
    // before the local PNG_Data container can be deallocated.
    // m_active_data = &png_data;

    read_IHDR_chunk();

    // If this image is palette indexed, handle it.
    if(m_is_indexed) handle_indexed_image();

    // Handle non palette indexed image.
    else handle_un_indexed_image();

    m_bin_parser.close_file();

    // m_active_data = nullptr;

    return png_data;
}


// Private

void Tundra::PNGLoader::read_IHDR_chunk()
{
    constexpr uint32_t STANDARD_IHDR_CHUNK_LENGTH = 13;

    uint32_t chunk_length = m_bin_parser.read_four_bytes_big_endian();
    uint32_t chunk_type = m_bin_parser.read_four_bytes_big_endian();

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

    // Allocate memory for the data of this chunk. This data pointer will be 
    // passed to libz after all data is collected to generate the proper crc
    // signature and compare it to the parsed crc signature at the end of the 
    // chunk to verify chunk integrity.
    uint8_t* chunk_data = new uint8_t[chunk_length];

    // Fetch image dimensions.
    m_active_data.image_width = m_bin_parser.read_four_bytes_big_endian();

    // Store back as big endian data.
    chunk_data[0] = m_active_data.image_width >> 24;
    chunk_data[1] = m_active_data.image_width >> 16;
    chunk_data[2] = m_active_data.image_width >> 8;
    chunk_data[3] = m_active_data.image_width;

    m_active_data.image_height = m_bin_parser.read_four_bytes_big_endian();

    // Store back as big endian data.
    chunk_data[4] = m_active_data.image_height >> 24;
    chunk_data[5] = m_active_data.image_height >> 16;
    chunk_data[6] = m_active_data.image_height >> 8;
    chunk_data[7] = m_active_data.image_height;

    // If the next byte, which is bit depth per channel of pixel is not 8.
    if(m_bin_parser.read_byte() != 8)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Bit depth per channel is"
        " not 8 bits. Only 8 bits channels are supported.\n";
        exit(-1);
    }

    // Bit depth must be 8,
    chunk_data[8] = 8;

    uint8_t color_type = m_bin_parser.read_byte();
    chunk_data[9] = color_type;

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
    chunk_data[10] = 0;

    // If the compression byte is not 0.
    if(m_bin_parser.read_byte() != 0)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Filter byte is not 0."
            << '\n';
        exit(-1);
    }
    chunk_data[11] = 0;

    // If the interlace byte is not 0.
    if(m_bin_parser.read_byte() != 0)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Interlace byte is not 0"
            << '\n';
        exit(-1);
    }
    chunk_data[12] = 0;

    uint32_t crc = m_bin_parser.read_four_bytes_big_endian();

    // If CRC verification failed.
    if(!verify_CRC(crc, chunk_length, chunk_type, chunk_data))
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : IHDR chunk failed "
            "integrity check.\n";
        exit(-1);
    }

    delete[] chunk_data;

    // Valid IHDR has been read.
}

void Tundra::PNGLoader::handle_indexed_image()
{
    // Get the PLTE chunk data.
    uint8_t* palette_entries = parse_PLTE_chunk();

    // Read uncompressed IDAT data.
    read_and_decompress_all_IDAT_data();
    
    // Unfilter the IDAT data.
    unfilter_IDAT_data();

    delete[] palette_entries;
}

void Tundra::PNGLoader::handle_un_indexed_image() {}

void Tundra::PNGLoader::unfilter_IDAT_data() const
{
    // for(uint32_t line = 0; line < m_active_data.image_height; ++line)
    // {
        
    // }
}

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

bool Tundra::PNGLoader::verify_CRC(uint32_t crc, uint32_t chunk_length, 
    uint32_t chunk_type, uint8_t* chunk_start) const
{
    // Reset libz computation by passing NULL.
    uLong libz_computed_crc = crc32(0L, Z_NULL, 0);

    // Chunk type with bytes flipped for big endian.
    uint8_t chunk_type_as_big_endian[4] = {
        static_cast<uint8_t>((chunk_type >> 24)),
        static_cast<uint8_t>((chunk_type >> 16)),
        static_cast<uint8_t>((chunk_type >> 8)),
        static_cast<uint8_t>((chunk_type))
    };

    // Compute correct crc from chunk type.
    libz_computed_crc = crc32(libz_computed_crc, chunk_type_as_big_endian, 4);

    // Continue to compute correct crc chunk from chunk data.
    libz_computed_crc = crc32(libz_computed_crc, chunk_start, chunk_length); 

    return libz_computed_crc == crc;
}

uint32_t Tundra::PNGLoader::find_PLTE_chunk_and_get_length()
{
    uint32_t chunk_length = m_bin_parser.read_four_bytes_big_endian();
    uint32_t chunk_type = m_bin_parser.read_four_bytes_big_endian();

    while(chunk_type != DECIMAL_PLTE_SIGNATURE)
    {
        if(chunk_type == DECIMAL_IEND_SIGNATURE)
        {
            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cerr << "PNGLoader::find_PLTE_chunk_and_get_length() : Reached end of PNG "
                "data and did not find PLTE chunk on palette indexed image.\n";
            exit(-1);
        }

        // This is a chunk we don't care about. Skip over its data and crc.
        for(uint32_t i = 0; i < chunk_length + 4; ++i)
        {
            m_bin_parser.read_byte();
        }
    }

    return chunk_length;
}

uint8_t* Tundra::PNGLoader::parse_PLTE_chunk()
{
    uint32_t PLTE_chunk_length = find_PLTE_chunk_and_get_length();
    
    // Palette chunk data is not divisible by 3, which is must be.
    if(PLTE_chunk_length % 3 != 0)
    {
        // @todo Replace this with a crash handler call, and use a 
        // TundraEngine callback to softly crash.
        std::cerr << "PNGLoader::handle_indexed_image() : Palette chunk is not "
            "divisible by 3, which it must be.\n";
        exit(-1);
    }

    // Allocate array memory for the palette entries. This will be kept in 
    // big endian format for the libz crc chunk integrity check.
    uint8_t* palette_entries = new uint8_t[PLTE_chunk_length];

    // Fetch all palette data.
    for(uint32_t i = 0; i < PLTE_chunk_length; ++i)
    {
        palette_entries[i] = m_bin_parser.read_byte();
    }

    uint32_t crc = m_bin_parser.read_four_bytes_big_endian();

    // If CRC verification failed. We can pass in the PLTE signature here as the
    // chunk since we know that the `find_PLTE_chunk_and_get_length()` already
    // asserted that the read signature from the PLTE chunk matched this 
    // definition signature.
    if(!verify_CRC(crc, PLTE_chunk_length, DECIMAL_PLTE_SIGNATURE, 
        palette_entries))
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::handle_indexed_image() : PLTE chunk failed "
            "integrity check.\n";
        exit(-1);
    }

    return palette_entries;
}
