#include "bus/Ram.hpp"
#include <cstring>

namespace slowstation::bus
{
    /**
     * @brief Constructor that initializes the system RAM to zero.
     * PS1 Main RAM is exactly 2MB.
     */
    Ram::Ram()
    {
        m_data.resize(RAM_SIZE, 0);
    }

    /**
     * @brief Standard 32-bit read from memory.
     * Extracts 4 bytes into a 32-bit word using Little-Endian order.
     */
    uint32_t Ram::read32(const uint32_t offset) const
    {
        uint32_t value;
        // Efficient memory access via memcpy to handle potential host alignment issues.
        std::memcpy(&value, &m_data[offset], sizeof(uint32_t));
        return value;
    }

    /**
     * @brief Standard 32-bit write to memory.
     * Stores a 32-bit word into 4 bytes using Little-Endian order.
     */
    void Ram::write32(const uint32_t offset, const uint32_t value)
    {
        std::memcpy(&m_data[offset], &value, sizeof(uint32_t));
    }

    /**
     * @brief Returns the total capacity of the RAM.
     */
    uint32_t Ram::getSize() const
    {
        return RAM_SIZE;
    }
} // namespace slowstation::bus
