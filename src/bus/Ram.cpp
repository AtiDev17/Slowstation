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
     * @brief Standard 8-bit read from memory.
     */
    uint8_t Ram::read8(const uint32_t offset) const
    {
        return m_data[offset];
    }

    /**
     * @brief Standard 16-bit read from memory.
     */
    uint16_t Ram::read16(const uint32_t offset) const
    {
        uint16_t value;
        std::memcpy(&value, &m_data[offset], sizeof(uint16_t));
        return value;
    }

    /**
     * @brief Standard 32-bit read from memory.
     */
    uint32_t Ram::read32(const uint32_t offset) const
    {
        uint32_t value;
        std::memcpy(&value, &m_data[offset], sizeof(uint32_t));
        return value;
    }

    /**
     * @brief Standard 8-bit write to memory.
     */
    void Ram::write8(const uint32_t offset, const uint8_t value)
    {
        m_data[offset] = value;
    }

    /**
     * @brief Standard 16-bit write to memory.
     */
    void Ram::write16(const uint32_t offset, const uint16_t value)
    {
        std::memcpy(&m_data[offset], &value, sizeof(uint16_t));
    }

    /**
     * @brief Standard 32-bit write to memory.
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
