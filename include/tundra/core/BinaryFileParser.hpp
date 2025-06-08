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
	 * The `buffer_size` specifies the size of the buffer that will be used to 
	 * cache in bytes from the file into RAM from disk. If a byte is requested 
	 * to be read that exceeds the cached bytes, a new buffer of bytes of 
	 * `buffer_size` is read in from the file. Set `buffer_size` to 0 to set 
	 * the buffer size to the entire file, and the whole file will be loaded
	 * into RAM. I found that a buffer size > 1KB did not make any 
	 * noticeable difference on byte read speed. If `buffer_size` is greater 
	 * than the file's size in bytes, the buffer will shrink to fit the size of 
	 * the file.
	 * 
	 * Attempting to open a file when one is already open will return false.
	 * 
	 * Returns true if the file was successfully opened and buffered. 
	 * 
	 * @param file_path Path to the file to open.
	 * @param buffer_size Size of buffered bytes.
	 */
	bool open_file(const char* file_path, std::size_t buffer_size = 1000);

	/**
	 * @brief Returns true if the end of the open file has been reached, and 
	 * there are no more available bytes to be read.
	 */
	bool eof() const;

	/**
	 * 
	 * @brief Read the next byte.
	 */
	uint8_t read_byte();

	uint16_t read_two_bytes();

	uint32_t read_three_bytes();

	uint32_t read_four_bytes();

	uint64_t read_eight_bytes();

	/**
	 * @brief Returns the open file's size in bytes.
	 */
	std::size_t get_file_byte_size() const;

	/**
	 * @brief Returns a read-only pointer to the internal buffer that holds 
	 * at least the requested number `num_bytes` bytes.
	 * 
	 * `num_bytes` can be bigger than the initially set buffer value. If the 
	 * buffer size was too small for this request, a new buffer with that size
	 * is allocated, then when the next `read_byte()` call is made (assuming
	 * there are more bytes in the file to fetch) the buffer size will 
	 * return to its normal size specified when `open_file` was originally 
	 * called.
	 * 
	 * If `num_bytes` does not exceed the number of bytes already loaded in 
	 * the buffer, a pointer is simply returned to the buffer where the 
	 * next byte would be returned, and there is guaranteed to be `num_bytes`
	 * bytes left in the buffer to read. This means that while there may be 
	 * valid bytes to read after the requested `num_bytes` starting from the 
	 * pointer, the user SHOULD NOT continue to read these bytes manually, as 
	 * the internal iterators assume that the user only read in their specified 
	 * `num_bytes`, and the next `read_byte()` call will start right after 
	 * the `num_bytes` in the buffer.
	 * 
	 * 
	 * @param num_bytes Const pointer to the internal buffer of bytes fetched.
	 */
	// const uint8_t* read_n_bytes(std::size_t num_bytes);

	/**
	 * @brief Reads all bytes from the open file, and returns a read-only pointer
	 * to the internal buffer that contains all the bytes.
	 * 
	 * This method returns ALL bytes from the file, regardless how many 
	 * bytes have already been read in, and where the internal byte iterator 
	 * is.
	 * 
	 * The user is not responsible for handling the memory since this is an 
	 * internal buffer, but please read the following warning:
	 * 
	 * @attention After this method is called, the bytes will stay cached in the 
	 * internal buffer until `close_file()` is called. After the close file 
	 * call, the internal buffer is deleted, and the pointer returned from this 
	 * function will point to invalid memory! Ensure to do all byte processing 
	 * before closing the file. Additionally, any buffers stored before this 
	 * function call (obtained from calls like `read_n_bytes()`) are now 
	 * invalid, since this new buffer created for the entire file will replace 
	 * the old buffer. 
	 */
	// const uint8_t* read_entire_open_file();

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
	void handle_buffer_creation(std::size_t buffer_size);

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
