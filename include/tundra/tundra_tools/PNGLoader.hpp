/**
 * @file PNGLoader.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>
#include <vector>

#include "tundra/tundra_tools/BinaryFileParser.hpp"


namespace Tundra
{

/**
 * @brief What format a pixel can take (How the bytes are structured that make
 * up a pixel).
 * 
 * A PIXEL_FORMAT is attached to pixel data such that the handler of the pixel
 * data knows how to parse and handle the pixels.
 * 
 */
enum class PIXEL_FORMAT : uint8_t
{
	GRAYSCALE,
	RGB,
	GRAYSCALE_ALPHA, 
	RGBA
};

/**
 * @brief Contains data for a PNG image that has been loaded, decompressed and
 * unfiltered.
 * 
 * Contains the raw pixel data of the reference image, format of said pixels in 
 * the pixel data as well as the dimensions of the reference image.
 */
struct PNG_Data
{
	std::vector<uint8_t> pixels;

	// Format of the pixels.
	PIXEL_FORMAT pixel_format = Tundra::PIXEL_FORMAT::RGBA; 

	uint8_t bytes_per_pixel {}; // Number of bytes that represent a single pixel.

	uint32_t image_width {}; // Width of the reference image.
	uint32_t image_height {}; // Height of the reference image.
};

/**
 * @brief Handles loading PNG images into an array of Tundra::Color objects 
 * which represent 
 * 
 */
class PNGLoader
{

public:

	PNGLoader();

	/**
	 * @brief Creates and returns a `PNG_Data` object from the PNG at the 
	 * passed path.
	 * 
	 * This object contains basic data about the image, including the pixels,
	 * pixel format and reference image dimensions.
	 * 
	 * @attention The user is responsible for the memory of the pixel array!
	 * 
	 * @param png_path Path to the PNG. 
	 */
	PNG_Data load_png(const char* png_path);

private:

	// # MEMBERS # ------------------------------------------------------------

	// If the current PNG file being handled is indexed using a palette.
	bool m_is_indexed = false;

	Tundra::BinaryFileParser m_bin_parser;


	// # METHODS # ------------------------------------------------------------

	/**
	 * @brief Reads in the IHDR chunk of the open file and retrieves PNG 
	 * metadata, storing it in the passed PNG Data object.
	 * 
	 * @param png_data PNG data container to modify.
	 */
	void read_IHDR_chunk(PNG_Data& png_data);
	
	/**
	 * @brief Calculates and sets the `bytes_per_pixel` member inside the passed
	 * PNG_Data object depending on the color type of the PNG data. 
	 * 
	 * @param png_data PNG data container to modify.
	 */
	void calculate_bytes_per_pixel(PNG_Data& png_data);

	/**
	 * @brief Iterates over the rest of the chunk data after the IHDR and
	 * finishes loading the passed PNG Data object with the context that an 
	 * indexed PNG is being handled.
	 *
	 * @param png_data PNG data container to modify.
	 */
	void handle_indexed_image(PNG_Data& png_data);

	/**
	 * @brief Iterates over the rest of the chunk data after the IHDR and
	 * finishes loading the passed PNG Data object with the context that a non 
	 * indexed PNG is being handled.
	 */
	void handle_un_indexed_image(PNG_Data& png_data);

	/**
	 * @brief Iterates over the PNG file, collects data from all IDAT chunks,
	 * decompresses said data, then stores it in the pixels vector of the passed
	 * PNG_Data object.
	 * 
	 * Assumes that the byte iterator is at the start of a chunk, so this method
	 * can read in the length and type.
	 * 
	 * When this function returns, the byte iterator will be at the end of the 
	 * file having parsed the IEND signature. No other bytes should be read in.
	 *
	 * @param png_data PNG data container to modify. 
	 */
	void read_and_decompress_all_IDAT_data(PNG_Data& png_data);

	/**
	 * @brief Parses the passed PNG data's pixels and unfilters each "line".
	 * 
	 * Pixels are modified directly inside the PNG data container.
	 * 
	 * @param png_data PNG data container to modify.
	 */
	void unfilter_IDAT_data(PNG_Data& png_data) const;

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this line was filtered using the 
	 * 'sub' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	void handle_line_with_sub_unfilter(PNG_Data& png_data, 
		uint32_t start_index) const;

	/**
	 * @brief Returns true if the first 8 bytes of the open file match the 
	 * known PNG signature.
	 */
	bool verify_PNG_signature();

	/**
	 * @brief Returns true if the given `crc`(Cyclic Redundancy Check) found in
	 * an image chunk is the same as the computed crc from the chunk contents.
	 * 
	 * @param crc Parsed crc value.
	 * @param chunk_length Length of chunk in bytes.
	 * @param chunk_type 4 byte chunk type signature.
	 * @param chunk_start Pointer to the start of the stored chunk data.
	 */
	bool verify_CRC(uint32_t crc, uint32_t chunk_length, uint32_t chunk_type, 
		uint8_t* chunk_start) const;

	/**
	 * @brief Finds the target chunk through direct file iteration and returns
	 * its length.
	 * 
	 * Returns 0 if the end of the PNG is parsed and the chunk was not found.
	 * 
	 * Assumes that the byte iterator in the open PNG file is placed right at 
	 * the start of an unkown chunk. This will be the first chunk that is 
	 * analyzed.
	 * 
	 * When this function returns with a non zero integer, the byte iterator in 
	 * the open PNG file will be on the first byte of the data of the target 
	 * chunk, the type and length 8 bytes have already been read in to compare 
	 * the chunk to its signature.
	 * 
	 * @param chunk_signature Chunk signature of the target chunk.
	 */
	uint32_t find_chunk_and_get_length(uint32_t chunk_signature);

	/**
	 * @brief Iterates over the data of the PLTE chunk, returning a pointer to 
	 * heap memory storing the PLTE chunk data.
	 * 
	 * Assumes that the byte iterator of the PNG file is right at the first byte
	 * of the PLTE chunk. 
	 * 
	 * This method also checks the crc integrity of the PLTE chunk and softly 
	 * crashes if it fails.
	 * 
	 * Caller handles the memory.
	 */
	uint8_t* parse_PLTE_chunk();

	/**
	 * @brief Inflates passed `compress_data` using libz and returns the 
	 * uncompressed data.
	 * 
	 * @param compressed_data Data to inflate.
	 */
	std::vector<uint8_t> inflate_decompressed_data(
		const std::vector<uint8_t>& compressed_data) const;

}; // Class PNGLoader

} // Namespace Tundra
