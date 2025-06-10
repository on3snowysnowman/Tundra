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

#include <tundra/tundra_tools/PNGLoader.hpp>

#include <iostream>

#include <zlib.h>

// Chunk signatures as a decimal representation of 4 byte integers rather 
// than the 4 bytes of ASCII characters.

#define DECIMAL_IHDR_SIGNATURE 1229472850
#define DECIMAL_PLTE_SIGNATURE 1347179589
#define DECIMAL_IDAT_SIGNATURE 1229209940
#define DECIMAL_IEND_SIGNATURE 1229278788

// Filter type options that can occur in a PNG file.

#define PNG_LINE_FILTER_NONE 0
#define PNG_LINE_FILTER_SUB 1
#define PNG_LINE_FILTER_UP 2
#define PNG_LINE_FILTER_AVERAGE 3
#define PNG_LINE_FILTER_PAETH 4

// Values of the byte that is read in from a PNG that flags what color type 
// the PNG file is.

#define PNG_GRAYSCALE_COLOR_TYPE_BYTE 0
#define PNG_RGB_COLOR_TYPE_BYTE 2
#define PNG_INDEXED_COLOR_TYPE_BYTE 3
#define PNG_GRAYSCALE_ALPHA_COLOR_TYPE_BYTE 4
#define PNG_RGBA_COLOR_TYPE_BYTE 6


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

    // PNG Data container that will be filled at as the open PNG is handled.
    PNG_Data png_data;

    read_IHDR_chunk(png_data);

    calculate_bytes_per_pixel(png_data);

    // If this image is palette indexed, handle it.
    if(m_is_indexed) handle_indexed_image(png_data);

    // Handle non palette indexed image.
    else handle_un_indexed_image(png_data);

    m_bin_parser.close_file();

    return png_data;
}


// Private

void Tundra::PNGLoader::read_IHDR_chunk(PNG_Data& png_data)
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
    png_data.image_width = m_bin_parser.read_four_bytes_big_endian();

    // Store back as big endian data for crc check.
    chunk_data[0] = png_data.image_width >> 24;
    chunk_data[1] = png_data.image_width >> 16;
    chunk_data[2] = png_data.image_width >> 8;
    chunk_data[3] = png_data.image_width;

    png_data.image_height = m_bin_parser.read_four_bytes_big_endian();

    // Store back as big endian data for crc check.
    chunk_data[4] = png_data.image_height >> 24;
    chunk_data[5] = png_data.image_height >> 16;
    chunk_data[6] = png_data.image_height >> 8;
    chunk_data[7] = png_data.image_height;

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
        case PNG_GRAYSCALE_COLOR_TYPE_BYTE:

            png_data.pixel_format = Tundra::PIXEL_FORMAT::GRAYSCALE;
            break;

        case PNG_RGB_COLOR_TYPE_BYTE:

            png_data.pixel_format = Tundra::PIXEL_FORMAT::RGB;
            break;

        case PNG_INDEXED_COLOR_TYPE_BYTE:

            png_data.pixel_format = Tundra::PIXEL_FORMAT::RGB;
            m_is_indexed = true;
            break;

        case PNG_GRAYSCALE_ALPHA_COLOR_TYPE_BYTE:

            png_data.pixel_format = Tundra::PIXEL_FORMAT::GRAYSCALE_ALPHA;
            break;

        case PNG_RGBA_COLOR_TYPE_BYTE:

            png_data.pixel_format = Tundra::PIXEL_FORMAT::RGBA;
            break;

        // Invalid color index.
        default:

            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cerr << "PNGLoader::read_IHDR_chunk() : Invalid color index "
            "read from PNG file.\n";
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

void Tundra::PNGLoader::calculate_bytes_per_pixel(PNG_Data& png_data)
{
    switch(png_data.pixel_format)
    {
        case Tundra::PIXEL_FORMAT::GRAYSCALE:   

            png_data.bytes_per_pixel = 1;
            break;
        
        case Tundra::PIXEL_FORMAT::RGB:   

            // RGB can be present in two situations, where the PNG is indexed 
            // or it is not indexed and has a color type of RGB. If the image 
            // is indexed, each "pixel" is an index into the palette table, and
            // is only represented by 1 byte. If the image is not indexed, each
            // pixel is an actual pixel represented by 3 bytes.

            if(m_is_indexed) png_data.bytes_per_pixel = 1;
            else png_data.bytes_per_pixel = 3;

            break;
    
        case Tundra::PIXEL_FORMAT::GRAYSCALE_ALPHA:   

            png_data.bytes_per_pixel = 2;
            break;
    
        case Tundra::PIXEL_FORMAT::RGBA:   

            png_data.bytes_per_pixel = 4;
            break;
    }
}

void Tundra::PNGLoader::handle_indexed_image(PNG_Data& png_data)
{
    // Get the PLTE chunk data.
    uint8_t* palette_entries = parse_PLTE_chunk();

    read_and_decompress_all_IDAT_data(png_data);
    unfilter_IDAT_data(png_data);

    delete[] palette_entries;
}

void Tundra::PNGLoader::handle_un_indexed_image(PNG_Data& png_data) 
{
    read_and_decompress_all_IDAT_data(png_data);
    unfilter_IDAT_data(png_data);
}

void Tundra::PNGLoader::read_and_decompress_all_IDAT_data(PNG_Data& png_data)
{
    uint32_t IDAT_chunk_length = find_chunk_and_get_length(
        DECIMAL_IDAT_SIGNATURE);

    // Used during the crc check when passing the png_data's pixel underlying
    // data pointer to the crc verification method. Since we're assembling 
    // all IDAT chunks, we only want to verify the crc for the chunk we have
    // just parsed, and that's where this variable comes in, to offset from the
    // beginning of that underlying vector data pointer.
    uint32_t total_bytes_iterated = 0;

    // While the find chunk function does not return 0, a valid IDAT chunk has
    // been found and its data should be parsed.
    while(IDAT_chunk_length != 0)
    {
        // Reserve space for this chunk to optimize vector allocation.
        png_data.pixels.reserve(png_data.pixels.size() + IDAT_chunk_length);

        // Iterate through bytes in this IDAT chunk and add it to the pixels.
        for(uint32_t i = 0; i < IDAT_chunk_length; ++i)
            png_data.pixels.push_back(m_bin_parser.read_byte());

        uint32_t crc = m_bin_parser.read_four_bytes_big_endian();

        if(!verify_CRC(crc, IDAT_chunk_length, DECIMAL_IDAT_SIGNATURE, 
            png_data.pixels.data() + total_bytes_iterated))
        {
            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cerr << "PNGLoader::read_and_decompress_all_IDAT_data() : " 
                "IDAT chunked failed crc integrity check.\n";
            exit(-1);
        }

        total_bytes_iterated += IDAT_chunk_length;

        // Attempt to find next IDAT chunk, if it exists.
        IDAT_chunk_length = find_chunk_and_get_length(DECIMAL_IDAT_SIGNATURE);
    }

    // No more IDAT chunks have exist, we should have reached the end of file.

    png_data.pixels = inflate_decompressed_data(png_data.pixels);
}

void Tundra::PNGLoader::unfilter_IDAT_data(PNG_Data& png_data) const
{
    uint32_t line_start_index;

    for(uint32_t line = 0; line < png_data.image_height; ++line)
    {
        // Add 1 to the width calculation to account for the filter byte.
        line_start_index = line * 
            ((png_data.image_width * png_data.bytes_per_pixel) + 1);

        std::cout << "Line: " << line << " : ";
            
        // Handle line filter type.
        switch(png_data.pixels.at(line_start_index))
        {
            case PNG_LINE_FILTER_NONE:
                std::cout << "No filter\n";
                // No filter, do not modify pixels.
                break;

            case PNG_LINE_FILTER_SUB:
            std::cout << "Sub filter\n";

                handle_line_with_sub_unfilter(png_data, line_start_index + 1);
                break;

            case PNG_LINE_FILTER_UP:
            std::cout << "Up filter\n";

                break;

            case PNG_LINE_FILTER_AVERAGE:
            std::cout << "Avg filter\n";

                break;

            case PNG_LINE_FILTER_PAETH:
            std::cout << "Paeth filter\n";

                break;

            default:

                std::cout << "INVALID FILTER: " << int(png_data.pixels.at(line * 
                    png_data.image_height)) << '\n';
                exit(0);
        }
    }
}

void Tundra::PNGLoader::handle_line_with_sub_unfilter(PNG_Data& png_data,
    uint32_t start_index) const
{
    // Iterate through each pixel in this line and unfilter it. Increment start
    // index by the bytes per pixel to skip the first pixel, which is not 
    // filtered since we are unfiltering with the sub unfilter and it has no 
    // left neighbor. 1 is also added to the bound constraint, 
    for(uint32_t i = start_index + png_data.bytes_per_pixel; 
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        png_data.pixels.at(i) = png_data.pixels.at(i) + 
            png_data.pixels.at(i - png_data.bytes_per_pixel);
    }
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

uint32_t Tundra::PNGLoader::find_chunk_and_get_length(uint32_t chunk_signature)
{
    uint32_t chunk_length = m_bin_parser.read_four_bytes_big_endian();
    uint32_t chunk_type = m_bin_parser.read_four_bytes_big_endian();

    std::cout << "Chunk signature found: " << char(chunk_type >> 24) << 
        char(chunk_type >> 16) << char(chunk_type >> 8) << char(chunk_type) << 
        '\n';

    while(chunk_type != chunk_signature)
    {
        // Reached end of file.
        if(chunk_type == DECIMAL_IEND_SIGNATURE)
        {
            return 0;
        }

        // This is a chunk we don't care about. Skip over its data and crc 
        // bytes.
        for(uint32_t i = 0; i < chunk_length + 4; ++i)
        {
            m_bin_parser.read_byte();
        }

        // Read in next chunk 
        chunk_length = m_bin_parser.read_four_bytes_big_endian();
        chunk_type = m_bin_parser.read_four_bytes_big_endian();

        std::cout << "Chunk signature found: " << char(chunk_type >> 24) << 
        char(chunk_type >> 16) << char(chunk_type >> 8) << char(chunk_type) << 
        '\n';
    }

    return chunk_length;
}

uint8_t* Tundra::PNGLoader::parse_PLTE_chunk()
{
    uint32_t PLTE_chunk_length = find_chunk_and_get_length(
        DECIMAL_PLTE_SIGNATURE);
    
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

std::vector<uint8_t> Tundra::PNGLoader::inflate_decompressed_data(
    const std::vector<uint8_t>& compressed_data) const
{
    z_stream stream {};

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.next_in = const_cast<Bytef*>(compressed_data.data());
    stream.avail_in = static_cast<uInt>(compressed_data.size());

    // Initialize inflate state.
    if(inflateInit(&stream) != Z_OK)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::inflate_decompressed_data() : Libz inflate "
            "init failed.\n";
        exit(-1);
    }

    std::vector<uint8_t> inflated_data(compressed_data.size() * 2);

    int ret;
    do 
    {

        if (stream.total_out >= inflated_data.size()) 
        {
            
            // Grow output buffer if needed.
            inflated_data.resize(inflated_data.size() * 2);
        }

        stream.next_out = inflated_data.data() + stream.total_out;
        stream.avail_out = static_cast<uInt>(inflated_data.size() - 
        stream.total_out);

        ret = inflate(&stream, Z_NO_FLUSH);

        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) 
        {
            inflateEnd(&stream);
            throw std::runtime_error("inflate failed");
        }

    } while (ret != Z_STREAM_END);

    // Trim unused space
    inflated_data.resize(stream.total_out);

    // Clean up
    inflateEnd(&stream);

    return inflated_data;
}
