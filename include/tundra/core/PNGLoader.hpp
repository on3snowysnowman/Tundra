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
 * @brief What color format a pixel can take (How the bytes are structured that 
 * make up a pixel).
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
 * the pixel data, bytes per pixel, as well as the dimensions of the reference 
 * image.
 */
struct PNG_Data
{
	std::vector<uint8_t> pixel_data;

	// Format of the pixels.
	PIXEL_FORMAT pixel_format = Tundra::PIXEL_FORMAT::RGBA; 

	uint8_t bytes_per_pixel {}; // Number of bytes that represent a single pixel.

	// Size in bytes of the reference image. This is the same as the 
	// calculation: image_width * image_height * bytes_per_pixel
	uint32_t image_size_in_bytes {};
	uint32_t image_width {}; // Width of the reference image.
	uint32_t image_height {}; // Height of the reference image.
};

/**
 * @brief Handles loading PNG files into raw pixel data.
 */
class PNGLoader
{

public:

	PNGLoader() =default;

	/**
	 * @brief Creates and returns a `PNG_Data` object filled with the data from 
	 * the PNG at the passed path.
	 * 
	 * This object contains basic data about the image, including the pixels,
	 * bytes per pixel, pixel format and reference image dimensions.
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
	void calculate_bytes_per_pixel(PNG_Data& png_data) const;

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
	static void unfilter_IDAT_data(PNG_Data& png_data);

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this line was filtered using the 
	 * 'sub' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param unfiltered_pixels Pixels that have already been unfiltered.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_line_with_sub_filter(PNG_Data& png_data, 
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);
	
	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this line was filtered using the 
	 * 'up' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_line_with_up_filter(PNG_Data& png_data, 
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this line was filtered using the 
	 * 'average' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param unfiltered_pixels Pixels that have already been unfiltered.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_line_with_average_filter(PNG_Data& png_data, 
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this is the first line in the image
	 * and was filtered using the 'average' filter type.
	 * 
	 * @param png_data PNG data container to modify.
	 * @param unfiltered_pixels Pixels that have already been unfiltered.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_first_line_with_average_filter(PNG_Data& png_data,
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this line was filtered using the 
	 * 'paeth' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param unfiltered_pixels Pixels that have already been unfiltered.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_line_with_paeth_filter(PNG_Data& png_data, 
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);

	/**
	 * @brief Unfilters the line inside the passed PNG data pixels at 
	 * `start_index` with the context that this is the first line in the image 
	 * and was filtered using the 'paeth' filter type. 
	 * 
	 * @param png_data PNG data container to modify.
	 * @param unfiltered_pixels Pixels that have already been unfiltered.
	 * @param start_index Starting position inside the pixel vector to begin
	 * 					  unfiltering.
	 */
	static void handle_first_line_with_paeth_filter(PNG_Data& png_data, 
		std::vector<uint8_t>& unfiltered_pixels, uint32_t start_index);

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
	static bool verify_CRC(uint32_t crc, uint32_t chunk_length, uint32_t chunk_type, 
		uint8_t* chunk_start);

	/**
	 * @brief Computes and returns the Paeth predictor per byte during byte 
	 * unfiltering.
	 *
	 * The Paeth predictor estimates the value of a byte based on three 
	 * neighboring bytes: left (`a`), above (`b`), and upper-left (`c`). It 
	 * selects the neighbor that is closest to the prediction `p = a + b - c`
	 * using absolute differences. This function is used to reverse the Paeth 
	 * filter during PNG scanline unfiltering.
	 *
	 * @param left The byte to the left of the target byte (Left).
	 * @param above The byte above the target byte (Up).
	 * @param upper_left The byte to the upper-left of the target byte (Upper-left).
	 * @return uint8_t The predicted byte value used to reverse the Paeth 
	 * filter.
	 */
	static uint8_t handle_paeth_prediction(uint8_t left, uint8_t above, 
		uint8_t upper_left);

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
