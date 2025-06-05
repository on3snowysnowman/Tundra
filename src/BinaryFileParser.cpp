/**
 * @file BinaryFileParser.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "core/BinaryFileParser.hpp"

#include <iostream>


// Constructors / Deconstructor

Tundra::BinaryFileParser::BinaryFileParser() {}


// Public

void Tundra::BinaryFileParser::close_file()
{
    // There is no open file.
    if(!m_open_file_path) return;

    m_in_file_stream.close();

    cleanup();
} 

bool Tundra::BinaryFileParser::open_file(const char* file_path, 
    std::size_t buffer_size)
{
    // If a file is already open, simply return false (for now).
    if(m_open_file_path) return false;

    m_in_file_stream.open(file_path, std::ios::binary);

    // If file stream failed to open.
    if(!m_in_file_stream.is_open()) return false;

    m_open_file_path = file_path;

    calculate_file_byte_size();
    handle_buffer_creation(buffer_size);
    fill_buffer(m_buffer_size);

    return true;
}

bool Tundra::BinaryFileParser::eof() const { return m_eof_reached; }

uint8_t Tundra::BinaryFileParser::read_next_byte()
{   
    // If the end of file has been reached.
    if(m_eof_reached)
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "[ERR] BinaryFileParser::read_next_byte() : Requested to "
            "read next byte when EOF has been reached.\n";
        exit(-1);
    }

    uint8_t retrieved_byte = m_byte_buffer[m_buffer_iterator++];

    // The buffer iterator has passed the bounds of the buffer. Load new bytes.
    if(m_buffer_iterator >= m_buffer_iterator_clamp) 
    {
        fill_buffer(m_buffer_size);
    }

    return retrieved_byte;
}

std::size_t Tundra::BinaryFileParser::get_file_byte_size() const
    { return m_file_total_byte_size; }

const uint8_t* Tundra::BinaryFileParser::read_n_bytes(std::size_t num_bytes)
{
    // The requested number of bytes is less than the bytes already loaded 
    // in memory. 
    if(num_bytes <= m_buffer_iterator_clamp - m_buffer_iterator)
    {
        // Retrieve starting value of the bytes read.
        const uint8_t* start_byte = &m_byte_buffer[m_buffer_iterator];

        // Increment the buffer iterator since the user has just fetched some 
        // bytes. 
        m_buffer_iterator += num_bytes;

        return start_byte;
    }

    // More bytes are requested than those loaded into the buffer. Allocate 
    // a new buffer size.
    resize_and_fill_buffer(num_bytes);

    // Return a pointer to the newly created buffer.
    return m_byte_buffer;
}


// Private

void Tundra::BinaryFileParser::cleanup()
{
    m_eof_reached = false;
    m_open_file_path = nullptr;
    m_buffer_size = 0;
    m_buffer_iterator = 0;
    m_buffer_iterator_clamp = 0;
    m_file_byte_iterator = 0;
    m_file_total_byte_size = 0;

    delete[] m_byte_buffer;
    m_byte_buffer = nullptr;
}

void Tundra::BinaryFileParser::calculate_file_byte_size()
{
    // Calculate file size in bytes.
    m_in_file_stream.seekg(0, std::ios::end);
    m_file_total_byte_size = m_in_file_stream.tellg();
    m_in_file_stream.seekg(0, std::ios::beg);
}

void Tundra::BinaryFileParser::handle_buffer_creation(std::size_t buffer_size)
{
    if(buffer_size == 0)
    {
        // Set buffer size to the size of the file in bytes.

        buffer_size = m_file_total_byte_size;
    }

    m_buffer_size = buffer_size;

    m_byte_buffer = new uint8_t[m_buffer_size];
    
    // Fill buffer with intiial bytes.
    // fill_buffer(m_buffer_size);
}

void Tundra::BinaryFileParser::fill_buffer(std::size_t num_bytes,
    std::size_t buffer_offset)
{
    // If the last byte has been read in from the file.
    if(m_file_byte_iterator >= m_file_total_byte_size)
    {
        // Flag the end of the file reached, do not fill the buffer.
        m_eof_reached = true;
        return;
    }

    // Initially set clamp to buffer size, check on next line.
    m_buffer_iterator_clamp = num_bytes;

    // If there aren't enough bytes left in the file to fill a full buffer.
    if(query_remaining_file_bytes() < num_bytes)
    {
        m_buffer_iterator_clamp = query_remaining_file_bytes();
    }

    // Update iterator positions.
    m_file_byte_iterator += m_buffer_iterator_clamp;
    m_buffer_iterator = 0;

    // Read in bytes from the file.
    m_in_file_stream.read(reinterpret_cast<char*>(m_byte_buffer + buffer_offset), 
        m_buffer_iterator_clamp);

    // If there was an error fetching the requested size of bytes.
    if (m_in_file_stream.gcount() != m_buffer_iterator_clamp) 
    {
        // @todo Replace this with a crash handler call, and use a TundraEngine
        // callback to softly crash.
        std::cerr << "[ERR] BinaryFileParser::fill_buffer() : Failed to "
            " retrive requested number of bytes.\n";
        exit(-1);
    }
}

void Tundra::BinaryFileParser::resize_and_fill_buffer(std::size_t num_bytes)
{
    // This method will only ever be called if the new buffer size is greater
    // than the old one, so that does not need to be handled here.

    // Allocate new buffer.
    uint8_t* new_buffer = new uint8_t[num_bytes];

    // Number of bytes that are remaining in the old buffer that can be copied 
    // into the new one.
    const std::size_t NUM_BYTES_TO_COPY = 
        m_buffer_iterator_clamp - m_buffer_iterator;
    
    // Iterate through any remaining bytes in the old buffer and copy them to 
    // the new one.
    for(std::size_t i = 0; i < NUM_BYTES_TO_COPY; ++i)
    {
        new_buffer[i] = m_byte_buffer[m_buffer_iterator];
    }

    // No memory leaks here!
    delete[] m_byte_buffer;

    // Set old buffer pointer to the new buffer.
    m_byte_buffer = new_buffer;

    // Fill the rest of the buffer with bytes. We can do so here by specifying
    // the buffer size to fill to be the local function requested number of 
    // bytes minus the ones we've already copied, then apply a pointer offset
    // equal to the number of bytes we've copied to tell the fill_buffer 
    // function to start there. 
    fill_buffer(num_bytes - NUM_BYTES_TO_COPY, NUM_BYTES_TO_COPY);
}


std::size_t Tundra::BinaryFileParser::query_remaining_file_bytes() const
    { return m_file_total_byte_size - m_file_byte_iterator; }
