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
	void* pixels = nullptr; // Array of pixels.

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

	// Temporary container for storing PNG data as a PNG file is handled. This
	// object is copied and returned after handling is complete. When no PNG 
	// is being handled, this object is in a valid, but empty state.
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
	 * @param chunk_start Pointer to the start of the chunk.
	 */
	bool verify_CRC(uint32_t crc, uint32_t chunk_length, uint32_t* chunk_start)
		const;

	/**
	 * @brief Searches for the PLTE chunk and returns the size of it.
	 * 
	 * Will softly crash the program if the PLTE chunk was not found. This 
	 * method is only called if it is known that the image is indexed, and 
	 * requires a PLTE chunk.
	 */
	uint32_t find_PLTE_chunk();

}; // Class PNGLoader

} // Namespace Tundra
