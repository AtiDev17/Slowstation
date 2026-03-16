#include "bus/Bios.hpp"
#include <fstream>
#include <stdexcept>

namespace slowstation::bus
{
    /**
     * @brief Constructor that initializes the BIOS and triggers the loading process.
     * @param path The filesystem path to the BIOS image.
     */
    Bios::Bios(const std::string& path)
    {
        load(path);
    }

    /**
     * @brief Implementation of IMemoryDevice::getSize.
     * @return The fixed BIOS size of 512KB.
     */
    uint32_t Bios::getSize() const
    {
        return BIOS_SIZE;
    }

    /**
     * @brief Reads a 32-bit word from the BIOS ROM.
     * 
     * Since the PlayStation 1 CPU is Little-Endian, we manually reconstruct 
     * the 32-bit word from 4 individual bytes. This ensures portability 
     * across different host architectures (x86, ARM, etc.).
     * 
     * @param offset The byte offset into the BIOS data.
     * @return The 32-bit word at the specified offset.
     * @throws std::out_of_range If the offset is outside the BIOS boundaries.
     */
    uint32_t Bios::read32(const uint32_t offset) const
    {
        // Ensure the read does not exceed the buffer limits (4 bytes required for a 32-bit read).
        if (offset + 3 >= m_data.size())
        {
            throw std::out_of_range("Bios: Attempted to read out of bounds at offset " + std::to_string(offset));
        }

        // Extract bytes in Little-Endian order (LSB first).
        uint32_t b0 = m_data[offset + 0];
        uint32_t b1 = m_data[offset + 1];
        uint32_t b2 = m_data[offset + 2];
        uint32_t b3 = m_data[offset + 3];

        // Shift and combine into a single 32-bit value.
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }

    /**
     * @brief Loads the BIOS binary file into memory with strict validation.
     * 
     * This follows RAII principles. We validate that the file exists and 
     * that its size matches the expected 512KB (524,288 bytes) exactly.
     * 
     * @param path The filesystem path to the BIOS image.
     * @throws std::runtime_error If the file cannot be opened, read, or has the wrong size.
     */
    void Bios::load(const std::string& path)
    {
        // Open file in binary mode and jump to the end (std::ios::ate) to check size immediately.
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw std::runtime_error("Bios: Failed to open file at path: " + path);
        }

        // The BIOS for PS1 must be exactly 512KB.
        std::streamsize size = file.tellg();
        if (size != BIOS_SIZE)
        {
            throw std::runtime_error(
                "Bios: Incorrect file size. Expected 512KB, but got " + std::to_string(size) + " bytes.");
        }

        // Reset file pointer to the beginning and read the data into the pre-allocated vector.
        file.seekg(0, std::ios::beg);
        m_data.resize(BIOS_SIZE);
        if (!file.read(reinterpret_cast<char*>(m_data.data()), BIOS_SIZE))
        {
            throw std::runtime_error("Bios: Failed to read data from file.");
        }
    }
}
