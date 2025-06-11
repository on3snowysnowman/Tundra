/**
 * @file BinaryFileParser.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <fstream>
#include <cstdint>


namespace Tundra
{

/**
 * @brief Handles parsing a file's raw binary data, allowing fetching individual
 * bytes.
 */
class BinaryFileParser
{

public:

	BinaryFileParser();

	/**
	 * @brief Closes the active open file, and clears the internal byte
	 * buffer. 
	 */
	void close_file();

	/**
	 * @brief Attempts to open a file and retrieve initial byte data.
	 * 
	 * `max_buffer_size` delegates the maximum size of the internal buffer that
	 * caches in bytes into RAM as they are parsed (Think of it like a cpu 
	 * cache). If the file size of the open file is less than `max_buffer_size`
	 * then the entire file will be read into RAM. Set this value to 0 to always
	 * read in the entire file. 
	 * 
	 * Attempting to open a file when one is already open will return false.
	 * 
	 * Returns true if the file was successfully opened and buffered. 
	 * 
	 * @param file_path Path to the file to open.
	 * @param max_buffer_size Maximum allowed buffer size.
	 */
	bool open_file(const char* file_path, std::size_t max_buffer_size = 1000);

	/**
	 * @brief Returns true if the end of the open file has been reached, and 
	 * there are no more available bytes to be read.
	 */
	bool eof() const;

	/**
	 * @brief Read the next byte.
	 */
	uint8_t read_byte();

	/**
	 * @brief Reads in two bytes as a uint16_t with big endian file format in 
	 * mind.
	 * 
	 * Bytes read in from file are flipped to reverse the big endian stored 
	 * layout.
	 */
	uint16_t read_two_bytes_big_endian();

	/**
	 * @brief Reads in three bytes as a uint32_t with big endian file format in
	 * mind.
	 * 
	 * Bytes read in from file are flipped to reverse the big endian stored 
	 * layout.
	 * 
	 * The first 8 bits of the uint32_t will be empty.
	 */
	uint32_t read_three_bytes_big_endian();

	/**
	 * @brief Reads in four bytes as a uint32_t with big endian file format in
	 * mind.
	 * 
	 * Bytes read in from file are flipped to reverse the big endian stored 
	 * layout.
	 */
	uint32_t read_four_bytes_big_endian();

	/**
	 * @brief Reads in eight bytes as a uint64_t with big endian file format in
	 * mind.
	 * 
	 * Bytes read in from file are flipped to reverse the big endian stored 
	 * layout.
	 */
	uint64_t read_eight_bytes_big_endian();

	/**
	 * @brief Reads in two bytes as a uint16_t with little endian file format in
	 * mind.
	 * 
	 * Bytes read in from file maintain their order since the file format is 
	 * little endian.
	 * 
	 */
	uint16_t read_two_bytes_little_endian();

	/**
	 * @brief Reads in two bytes as a uint32_t with little endian file format in
	 * mind.
	 * 
	 * Bytes read in from file maintain their order since the file format is 
	 * little endian.
	 * 
	 * The first 8 bits of the uint32_t will be empty.
	 */
	uint32_t read_three_bytes_little_endian();

	/**
	 * @brief Reads in two bytes as a uint32_t with little endian file format in
	 * mind.
	 * 
	 * Bytes read in from file maintain their order since the file format is 
	 * little endian.
	 * 
	 */
	uint32_t read_four_bytes_little_endian();

	/**
	 * @brief Reads in two bytes as a uint64_t with little endian file format in
	 * mind.
	 * 
	 * Bytes read in from file maintain their order since the file format is 
	 * little endian.
	 * 
	 */
	uint64_t read_eight_bytes_little_endian();

	/**
	 * @brief Returns the open file's size in bytes.
	 */
	std::size_t get_file_byte_size() const;

	/**
	 * @brief Returns the number of bytes left in the file starting from the 
	 * current parsed byte.
	 */
	std::size_t query_remaining_file_bytes() const;

private:

	// # MEMBERS # ------------------------------------------------------------

	bool m_eof_reached = false;

	// Path to the active open file, if there is one.
	const char* m_open_file_path = nullptr;

	// Size of the buffer that cached bytes from the file. 
	std::size_t m_buffer_size = 0;

	// Index of the current parsed byte in the buffer.
	std::size_t m_buffer_iterator = 0;

	// The index that the buffer iterator should stop at and request new buffer 
	// of bytes. The reason why the `m_buffer_size` value is not simply used to 
	// check when to stop, is if the final buffer is read in from the file, but 
	// there aren't enough bytes in the file to entirely fill the buffer, then 
	// the buffer will be left partially empty and a manual iterator bound must 
	// be placed, hence this variable.
	std::size_t m_buffer_iterator_clamp = 0;

	// Index of the position inside the file where the next buffer will be 
	// read in from. This is incremented by `m_buffer_size` each time a buffer
	// is read in.
	std::size_t m_file_byte_iterator = 0;

	// Size of bytes of the open file.
	std::size_t m_file_total_byte_size = 0;

	// Buffer of cached bytes from the open file.
	uint8_t* m_byte_buffer = nullptr;

	std::ifstream m_in_file_stream;


	// # METHODS # ------------------------------------------------------------

	/**
	 * @brief Resets internal tracked components to default states and deletes
	 * the byte buffer from heap.
	 */
	void cleanup();

	/**
	 * @brief Calculates the opened file size in bytes and stores it in 
	 * `m_file_total_byte_size`.
	 */
	void calculate_file_byte_size();

	/**
	 * @brief Handles initial creation of the buffer on heap.
	 * 
	 * If `buffer_size` is 0, the file size will be used as the buffer size.
	 * 
	 * @param buffer_size Size of buffer to create.
	 */
	void handle_init_buffer_creation(std::size_t buffer_size);

	/**
	 * @brief Handles incrementing the file byte iterator and checking if the 
	 * end of file has been reached.
	 * 
	 * @param increment_amount Amount to increment, default to 1.
	 */
	// void handle_file_iterator_increment(std::size_t increment_amount = 1);

	/**
	 * @brief Checks if a file is currently open, and if there is not, softly
	 * error outputs and crashes.
	 */
	void handle_file_open_check();

	/**
	 * @brief Loads in `m_buffer_size` bytes from the open file.
	 * 
	 * Automatically checks the remaining bytes in the file before reading, 
	 * and sets the `m_buffer_iterator_clamp` accordingly to the actual obtained
	 * bytes. Also sets the internal eof bool if the end of the open file has
	 * been reached.
	 *
	 * Optionally, the `buffer_offset` can be used to start filling bytes at 
	 * an offset position of the byte buffer. This is mainly used inside the 
	 * `resize_and_fill_buffer` method when a new buffer is created, and there
	 * still may be remaining bytes in the old buffer that are copied in, and 
	 * we don't want to overwrite them here.
	 * 
	 * @param num_bytes Number of bytes to fill the buffer with.
	 * @param buffer_offset Offset to apply to the buffer when loading bytes.
	 */
	void fill_buffer(std::size_t num_bytes, std::size_t buffer_offset = 0);

	/**
	 * @brief Resizes the buffer on heap and fills it with `num_bytes` bytes.
	 * 
	 * Automatically checks the previous buffer for content, and copies that 
	 * into the new buffer.
	 * 
	 * @param num_bytes Number of bytes to fill.
	 */
	void resize_and_fill_buffer(std::size_t num_bytes);

}; // Class BinaryFileParser

} // Namespace Tundra
