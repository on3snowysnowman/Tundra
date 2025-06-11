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
#include <algorithm>
#include <fstream>

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


// The only bit depth value supported by the Loader (For now).
constexpr uint8_t SUPPORTED_BIT_DEPTH = 8;


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

    png_data.image_size_in_bytes = png_data.image_width * png_data.image_height
        * png_data.bytes_per_pixel;

    // If this image is palette indexed, handle it.
    if(m_is_indexed) 
    {
        // handle_indexed_image(png_data);
        std::cout << "Indexed PNG files are currently not supported.\n";
    }

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

    // If the bit depth value does not match the supported bit depth.
    if(m_bin_parser.read_byte() != SUPPORTED_BIT_DEPTH)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "PNGLoader::read_IHDR_chunk() : Bit depth per channel is"
        " not 8 bits. Only 8 bits channels are supported.\n";
        exit(-1);
    }

    // Bit depth must be 8,
    chunk_data[8] = SUPPORTED_BIT_DEPTH;

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
        png_data.pixel_data.reserve(png_data.pixel_data.size() + IDAT_chunk_length);

        // Iterate through bytes in this IDAT chunk and add it to the pixels.
        for(uint32_t i = 0; i < IDAT_chunk_length; ++i)
            png_data.pixel_data.push_back(m_bin_parser.read_byte());

        uint32_t crc = m_bin_parser.read_four_bytes_big_endian();

        if(!verify_CRC(crc, IDAT_chunk_length, DECIMAL_IDAT_SIGNATURE, 
            png_data.pixel_data.data() + total_bytes_iterated))
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

    png_data.pixel_data = inflate_decompressed_data(png_data.pixel_data);
}

void Tundra::PNGLoader::unfilter_IDAT_data(PNG_Data& png_data) const
{
    // Variable that procedurally stores the starting index of each scanline
    // as they are parsed. Initially set to 1 to skip the first filter byte that
    // should not be handed off the unfilter functions, since they expect the 
    // index to start at the beginning of the image pixel data. 
    uint32_t line_start_index = 1;

    std::vector<uint8_t> unfiltered_pixels;

    // Reserve enough space for the pixels, subtracting the image height size,
    // as this will deduct the number of filter bytes that exist in the filtered
    // image, and the filter bytes aren't included in the final unfiltered 
    // pixel vector.
    unfiltered_pixels.reserve(png_data.pixel_data.size() - 
        png_data.image_height);

    // Check to make sure first filter byte is not PNG_LINE_FILTER_UP, 
    // PNG_LINE_FILTER_AVERAGE or PNG_LINE_FILTER PAETH since these rely on 
    // a scanline of bytes above but there are no bytes above because this is
    // the first scanline in the image.
    if(png_data.pixel_data.at(0) == PNG_LINE_FILTER_UP ||
       png_data.pixel_data.at(0) == PNG_LINE_FILTER_AVERAGE ||
       png_data.pixel_data.at(0) == PNG_LINE_FILTER_PAETH)
    {
        // @todo Replace this with a crash handler call, and use a 
        // TundraEngine callback to softly crash.
        std::cerr << "PNGLoader::unfilter_IDAT_data() : Invalid filter type -> "
            "Filter type for the first row cannot be of type 'up', 'average' or "
            "'paeth'";
        exit(-1);
    }

    // Handle the first scanline separately first, since this requires special 
    // algorithms for filters such as 'up', 'average' and 'paeth' since they 
    // require above pixels for their normal calculations. 

    // Deduce the first scanline filter from the first byte of the data.
    switch(png_data.pixel_data.at(0)) 
    {
        case PNG_LINE_FILTER_NONE:
            
            // No filter, do not modify pixels, simply copy the raw filtered 
            // pixels.
            std::copy_n(png_data.pixel_data.begin() + line_start_index,
                png_data.image_width * png_data.bytes_per_pixel, 
                std::back_inserter(unfiltered_pixels));
            break;

        case PNG_LINE_FILTER_SUB:

            handle_line_with_sub_filter(png_data, unfiltered_pixels, 
                line_start_index);
            break;

        case PNG_LINE_FILTER_UP:

            // This is the first scanline of the image, there are no above 
            // bytes for this line's pixels to sample from so just copy the 
            // raw filtered pixels.
            std::copy_n(png_data.pixel_data.begin() + line_start_index,
                png_data.image_width * png_data.bytes_per_pixel, 
                std::back_inserter(unfiltered_pixels));
            break;

        case PNG_LINE_FILTER_AVERAGE:

            // Call the special 'first line' method here since this is the 
            // first scanline in the image and there are no above pixels to 
            // sample from.
            handle_first_line_with_average_filter(png_data, unfiltered_pixels, 
                line_start_index);
            break;

        case PNG_LINE_FILTER_PAETH:

            // Call the special 'first line' method here since this is the 
            // first scanline in the image and there are no above pixels to 
            // sample from.
            handle_first_line_with_paeth_filter(png_data, unfiltered_pixels, 
                line_start_index);
            break;

        default:

            // @todo Replace this with a crash handler call, and use a 
            // TundraEngine callback to softly crash.
            std::cout << "INVALID FILTER: " << 
                int(png_data.pixel_data.at(0)) 
                << '\n';
            exit(0);
    }

    // Start the line index at 1 here since we handled the first line (0th 
    // index) previously. 
    for(uint32_t line = 1; line < png_data.image_height; ++line)
    {
        // Add 1 to the width calculation to account for the filter byte.
        line_start_index = line * 
            ((png_data.image_width * png_data.bytes_per_pixel) + 1);

        // Handle line filter type. Increment the line start index to skip over 
        // the filter byte before handing the start index to a filter handler 
        // function.
        switch(png_data.pixel_data.at(line_start_index++))
        {
            case PNG_LINE_FILTER_NONE:
                
                // No filter, do not modify pixels, simply copy the raw pixels.
                std::copy_n(png_data.pixel_data.begin() + line_start_index,
                    png_data.image_width * png_data.bytes_per_pixel, 
                    std::back_inserter(unfiltered_pixels));
                break;

            case PNG_LINE_FILTER_SUB:

                handle_line_with_sub_filter(png_data, unfiltered_pixels, 
                    line_start_index);
                break;

            case PNG_LINE_FILTER_UP:

                handle_line_with_up_filter(png_data, unfiltered_pixels,
                    line_start_index);
                break;

            case PNG_LINE_FILTER_AVERAGE:

                handle_line_with_average_filter(png_data, unfiltered_pixels, 
                    line_start_index);
                break;

            case PNG_LINE_FILTER_PAETH:

                handle_line_with_paeth_filter(png_data, unfiltered_pixels, 
                    line_start_index);
                break;

            default:

                // @todo Replace this with a crash handler call, and use a 
                // TundraEngine callback to softly crash.
                std::cout << "INVALID FILTER: " << 
                    int(png_data.pixel_data.at(line * png_data.image_height)) 
                    << '\n';
                exit(0);
        }
    }

    // Replace filtered data with the complete unfiltered data.
    png_data.pixel_data = std::move(unfiltered_pixels);
}

void Tundra::PNGLoader::handle_line_with_sub_filter(PNG_Data& png_data,
    std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index) const
{
    // Add the first pixel to the unfiltered pixels vector, since we are not 
    // modifying it as it is the first pixel and has no left neighbor.
    for(uint32_t i = start_index; i < start_index + png_data.bytes_per_pixel; 
        ++i)
    {
        unfiltered_pixels.push_back(png_data.pixel_data.at(i));
    }

    // Iterate through each byte in this line and unfilter it. Increment start
    // index by the bytes per pixel to skip the first pixel, which is not 
    // filtered since we are unfiltering with the sub unfilter and it has no 
    // left neighbor.
    for(uint32_t i = start_index + png_data.bytes_per_pixel; 
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        unfiltered_pixels.push_back(
        (
            png_data.pixel_data.at(i) +
            *(unfiltered_pixels.end() - png_data.bytes_per_pixel)
        ) % 256);
    }
}

void Tundra::PNGLoader::handle_line_with_up_filter(PNG_Data& png_data,
    std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index) const
{
    // Iterate through each byte in this line and calculate the unfiltered 
    // byte from the addition between this parsed byte and the byte directly
    // above.
    for(uint32_t i = start_index; 
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        uint8_t above_byte_value = *(unfiltered_pixels.end() - 
            (png_data.image_width * png_data.bytes_per_pixel));

        unfiltered_pixels.push_back(
        (
            png_data.pixel_data.at(i) + 
            above_byte_value
        ) % 256);
    }
}

void Tundra::PNGLoader::handle_line_with_average_filter(PNG_Data& png_data, 
    std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index) const
{
    // Add the bytes of the first pixel while only averaging in the above byte, 
    // since there is no left neighboring byte.
    for(uint32_t i = start_index; i < start_index + png_data.bytes_per_pixel; 
        ++i)
    {
        uint8_t above_byte_value = *(unfiltered_pixels.end() - 
            (png_data.image_width * png_data.bytes_per_pixel));

        // Add the average of the left byte and byte above to the byte at 
        // the current position.
        uint8_t filtered_value = 
        (   
            png_data.pixel_data.at(i) + 
            (above_byte_value >> 1)
        ) % 256;

        unfiltered_pixels.push_back(filtered_value);
    }

    // Iterate through each byte in this line and calculate the unfiltered 
    // byte from the addition between this parsed byte, the left byte and the 
    // byte above. 
    for(uint32_t i = start_index + png_data.bytes_per_pixel;
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        uint8_t above_byte_value = *(unfiltered_pixels.end() - 
            (png_data.image_width * png_data.bytes_per_pixel));

        uint8_t left_byte_value = *(unfiltered_pixels.end() -   
            png_data.bytes_per_pixel);

        // Add the average of the left byte and byte above to the byte at 
        // the current position.
        uint8_t filtered_value = 
        (   
            png_data.pixel_data.at(i) + // Filtered byte value.
            ((above_byte_value + left_byte_value) >> 1) // Average left and up.
        ) % 256;

        unfiltered_pixels.push_back(filtered_value);
    } 
}

void Tundra::PNGLoader::handle_first_line_with_average_filter(
    PNG_Data& png_data, std::vector<uint8_t>& unfiltered_pixels, 
    uint32_t start_index) const
{
    // Add the bytes of the first pixel as itself, since this pixel has no left 
    // or above pixel to average from.
    for(uint32_t i = start_index; i < start_index + png_data.bytes_per_pixel; 
        ++i)
    {
        unfiltered_pixels.push_back(png_data.pixel_data.at(i));
    }

    // Iterate through each byte in this line and calculate the unfiltered 
    // byte from the addition between this parsed byte and the left byte. We 
    // skip above here since this is the first scanline in the image and does
    // not have an above neighbor.
    for(uint32_t i = start_index + png_data.bytes_per_pixel;
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        uint8_t left_byte_value = *(unfiltered_pixels.end() -   
            png_data.bytes_per_pixel);

        // Add the average of the left byte and byte above to the byte at 
        // the current position.
        uint8_t filtered_value = 
        (   
            png_data.pixel_data.at(i) + // Filtered byte value.
            (left_byte_value >> 1) // Average left and up.
        ) % 256;

        unfiltered_pixels.push_back(filtered_value);
    } 
}

void Tundra::PNGLoader::handle_line_with_paeth_filter(PNG_Data& png_data,
    std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index) const
{
    // Add the bytes of the first pixel while only handling the above byte in
    // our paeth calculation, since there is no left neighboring bytes.
    for(uint32_t i = start_index; i < start_index + png_data.bytes_per_pixel; 
        ++i)
    {
        uint8_t above_byte_value = *(unfiltered_pixels.end() - 
            (png_data.image_width * png_data.bytes_per_pixel));

        unfiltered_pixels.push_back(png_data.pixel_data.at(i) + 
        handle_paeth_prediction(
            0, 
            above_byte_value,
            0));
    }

    // Iterate through each byte in this line and calculate the unfiltered 
    // byte from the addition between this parsed byte, and the paeth 
    // calculation from the left, above and left above bytes.
    for(uint32_t i = start_index + png_data.bytes_per_pixel;
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        uint8_t above_byte_value = *(unfiltered_pixels.end() - 
            (png_data.image_width * png_data.bytes_per_pixel));

        uint8_t above_left_byte_value = *(unfiltered_pixels.end() - 
            ((png_data.image_width + 1) * png_data.bytes_per_pixel));

        uint8_t left_byte_value = *(unfiltered_pixels.end() -   
            png_data.bytes_per_pixel);

        unfiltered_pixels.push_back(
        (
            png_data.pixel_data.at(i) + 
            handle_paeth_prediction(
                left_byte_value, 
                above_byte_value,
                above_left_byte_value)
        ) % 256);
    } 
}

void Tundra::PNGLoader::handle_first_line_with_paeth_filter(PNG_Data& png_data,
    std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index) const
{
    // Add the bytes of the first pixel as itself, since this pixel has no left
    // or above pixel to calculate from.
    for(uint32_t i = start_index; i < start_index + png_data.bytes_per_pixel; 
        ++i)
    {
        unfiltered_pixels.push_back(png_data.pixel_data.at(i));
    }

    // Iterate through each byte in this line and calculate the unfiltered 
    // byte from the addition between this parsed byte, and the paeth 
    // calculation from the left byte. We skip the above and left above bytes
    // here since this is the first scanline in the image and has no above 
    // neighbors.
    for(uint32_t i = start_index + png_data.bytes_per_pixel;
        i < start_index + (png_data.image_width * png_data.bytes_per_pixel); 
        ++i)
    {
        uint8_t left_byte_value = *(unfiltered_pixels.end() -   
            png_data.bytes_per_pixel);

        unfiltered_pixels.push_back(
        (
            png_data.pixel_data.at(i) + 
            handle_paeth_prediction(
                left_byte_value, 
                0,
                0)
        ) % 256);
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

uint8_t Tundra::PNGLoader::handle_paeth_prediction(uint8_t a, uint8_t b, 
    uint8_t c) const
{
    int p = int(a) + int(b) - int(c);  // initial estimate
    int pa = abs(p - int(a));
    int pb = abs(p - int(b));
    int pc = abs(p - int(c));

    if (pa <= pb && pa <= pc) return a;
    else if (pb <= pc) return b;
    
    return c;
}

uint32_t Tundra::PNGLoader::find_chunk_and_get_length(uint32_t chunk_signature)
{
    uint32_t chunk_length = m_bin_parser.read_four_bytes_big_endian();
    uint32_t chunk_type = m_bin_parser.read_four_bytes_big_endian();

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
