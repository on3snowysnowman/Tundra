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

#include "tundra/core/BinaryFileParser.hpp"


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
enum class PIXEL_FORMAT
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

	uint32_t image_width; // Width of the reference image.
	uint32_t image_height; // Height of the reference image.
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

	PNG_Data m_active_data;

	Tundra::BinaryFileParser m_bin_parser;


	// # METHODS # ------------------------------------------------------------

	/**
	 * @brief Reads in the IHDR chunk of the open file and retrieves PNG 
	 * metadata.
	 */
	void read_IHDR_chunk();
	
	/**
	 * @brief Iterates over the rest of the chunk data after the IHDR and
	 * finishes loading the PNG with the context that it is indexed.
	 */
	void handle_indexed_image();

	/**
	 * @brief Iterates over the rest of the chunk data after the IHDR and
	 * finishes loading the PNG with the context that it is not indexed.
	 */
	void handle_un_indexed_image();

	/**
	 * @brief Iterates over the PNG file, collects data from all IDAT chunks,
	 * decompresses said data, then stores it in the `m_active_data` pixels
	 * vector.
	 * 
	 * Assumes that the byte iterator is at the start of a chunk, so this method
	 * can read in the length and type.
	 * 
	 * When this function returns, the byte iterator will be at the end of the 
	 * file having parsed the IEND signature. No other bytes should be read in. 
	 */
	void read_and_decompress_all_IDAT_data();

	/**
	 * @brief Parses the IDAT data from the `m_active_data` pixels vector
	 * and unfilters it.
	 */
	void unfilter_IDAT_data() const;

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
	 * @brief Searches for the PLTE chunk and returns the length of it.
	 * 
	 * Assumes that the byte iterator is at the start of a chunk, so this method
	 * can read in the length and type.
	 * 
	 * Will softly crash the program if the PLTE chunk was not found. This 
	 * method is only called if it is known that the image is indexed, and 
	 * requires a PLTE chunk.
	 * 
	 * When this function returns, the byte iterator inside the PNG file will be 
	 * on the first byte of the PLTE chunk data, since the chunk type and length
	 * have already been read in.
	 */
	uint32_t find_PLTE_chunk_and_get_length();

	/**
	 * @brief Iterates over the data of the PLTE chunk, return a pointer to heap
	 * memory storing the PLTE chunk data.
	 * 
	 * Assumes that the byte iterator of the PNG file is right at the first byte
	 * of the PLTE chunk. 
	 * 
	 * This method also checks the crc integrity of the PLTE chunk and softly 
	 * crashes if it fails.
	 * 
	 * @return uint8_t* 
	 */
	uint8_t* parse_PLTE_chunk();

}; // Class PNGLoader

} // Namespace Tundra
