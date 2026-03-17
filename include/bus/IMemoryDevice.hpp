#pragma once
#include <cstdint>

namespace slowstation::bus
{
    /**
     * @brief IMemoryDevice - The interface for all hardware mapped to the system bus.
     *
     * This follows the SOLID Dependency Inversion Principle (DIP).
     * Instead of the Bus depending on concrete classes like Bios or Ram,
     * it depends on this abstraction.
     */
    class IMemoryDevice
    {
    public:
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~IMemoryDevice() = default;

        /**
         * @brief Reads an 8-bit byte from a specific offset.
         * @param offset Relative byte offset.
         * @return The 8-bit value.
         */
        [[nodiscard]] virtual uint8_t read8(uint32_t offset) const = 0;

        /**
         * @brief Reads a 16-bit halfword from a specific offset.
         * @param offset Relative byte offset.
         * @return The 16-bit value.
         */
        [[nodiscard]] virtual uint16_t read16(uint32_t offset) const = 0;

        /**
         * @brief Reads a 32-bit word from a specific offset within the device.
         * @param offset The relative byte offset from the start of the device's memory.
         * @return The 32-bit value stored at that offset.
         */
        [[nodiscard]] virtual uint32_t read32(uint32_t offset) const = 0;

        /**
         * @brief Writes an 8-bit byte to a specific offset.
         * @param offset Relative byte offset.
         * @param value The 8-bit value to store.
         */
        virtual void write8(uint32_t offset, uint8_t value) = 0;

        /**
         * @brief Writes a 16-bit halfword to a specific offset.
         * @param offset Relative byte offset.
         * @param value The 16-bit value to store.
         */
        virtual void write16(uint32_t offset, uint16_t value) = 0;

        /**
         * @brief Writes a 32-bit word to a specific offset within the device.
         * @param offset The relative byte offset from the start of the device's memory.
         * @param value The 32-bit value to write.
         */
        virtual void write32(uint32_t offset, uint32_t value) = 0;

        /**
         * @brief Returns the total size of this device in bytes.
         * @return Total memory-mapped size.
         */
        [[nodiscard]] virtual uint32_t getSize() const = 0;
    };
} // namespace slowstation::bus
