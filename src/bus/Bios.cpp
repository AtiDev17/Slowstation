#include "bus/Bios.hpp"
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <format>

namespace slowstation::bus
{
    /**
     * @brief Constructor that initializes the BIOS ROM from a binary image.
     */
    Bios::Bios(const std::string& path)
    {
        load(path);
    }

    /**
     * @brief Returns the fixed BIOS ROM size.
     */
    uint32_t Bios::getSize() const
    {
        return BIOS_SIZE;
    }

    /**
     * @brief Standard 32-bit read from ROM.
     * Extracts 4 bytes into a 32-bit word using Little-Endian order.
     */
    uint32_t Bios::read32(const uint32_t offset) const
    {
        if (offset + sizeof(uint32_t) > m_data.size())
        {
            throw std::out_of_range(std::format("Bios: Attempted out-of-bounds read at offset 0x{:08X}", offset));
        }

        uint32_t value;
        std::memcpy(&value, &m_data[offset], sizeof(uint32_t));
        return value;
    }

    /**
     * @brief Standard 32-bit write. Writing to the BIOS is generally ignored on the PS1.
     */
    void Bios::write32(uint32_t offset, uint32_t value)
    {
        // NO-OP: The BIOS is a Read-Only Memory.
    }

    /**
     * @brief Loads the BIOS ROM binary from disk into the internal buffer.
     */
    void Bios::load(const std::string& path)
    {
        // Open file and seek to the end to verify size.
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw std::runtime_error(std::format("Bios: Failed to open ROM image at path: {}", path));
        }

        const std::streamsize size = file.tellg();
        if (size != BIOS_SIZE)
        {
            throw std::runtime_error(std::format("Bios: Incorrect ROM size. Expected {} bytes, but found {}.", BIOS_SIZE, size));
        }

        // Return to the beginning and read the entire ROM into memory.
        file.seekg(0, std::ios::beg);
        m_data.resize(BIOS_SIZE);
        if (!file.read(reinterpret_cast<char*>(m_data.data()), BIOS_SIZE))
        {
            throw std::runtime_error("Bios: Failed to read ROM data from disk.");
        }
    }
} // namespace slowstation::bus
