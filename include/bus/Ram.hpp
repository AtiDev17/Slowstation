#pragma once
#include <vector>
#include <cstdint>
#include "IMemoryDevice.hpp"

namespace slowstation::bus
{
    /**
     * @class Ram
     * @brief Represents the 2MB Main RAM of the PlayStation 1.
     *
     * This class implements the IMemoryDevice interface to provide
     * read/write access to the system's main memory. Main RAM is
     * initialized to zero on system power-on.
     */
    class Ram : public IMemoryDevice
    {
    public:
        /**
         * @brief Constructor that initializes 2MB of memory to zero.
         */
        Ram();

        /**
         * @brief Standard 8-bit read from RAM.
         */
        [[nodiscard]] uint8_t read8(uint32_t offset) const override;

        /**
         * @brief Standard 16-bit read from RAM.
         */
        [[nodiscard]] uint16_t read16(uint32_t offset) const override;

        /**
         * @brief Standard 32-bit read from RAM.
         * @param offset The byte offset into RAM (0 to 2MB - 4).
         * @return A 32-bit word in Little-Endian format.
         */
        [[nodiscard]] uint32_t read32(uint32_t offset) const override;

        /**
         * @brief Standard 8-bit write to RAM.
         */
        void write8(uint32_t offset, uint8_t value) override;

        /**
         * @brief Standard 16-bit write to RAM.
         */
        void write16(uint32_t offset, uint16_t value) override;

        /**
         * @brief Standard 32-bit write to RAM.
         * @param offset The byte offset into RAM.
         * @param value The 32-bit value to store.
         */
        void write32(uint32_t offset, uint32_t value) override;

        /**
         * @brief Returns the total size of RAM.
         * @return The size of RAM (2MB).
         */
        [[nodiscard]] uint32_t getSize() const override;

    private:
        /// Internal storage for the 2MB of system memory.
        std::vector<uint8_t> m_data;

        /// Total size of the Main RAM in bytes.
        static constexpr uint32_t RAM_SIZE = 2 * 1024 * 1024; // 2 MB
    };
} // namespace slowstation::bus
