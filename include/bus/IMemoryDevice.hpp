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
         * @brief Reads a 32-bit word from a specific offset within the device.
         * @param offset The relative byte offset from the start of the device's memory.
         * @return The 32-bit value stored at that offset.
         */
        [[nodiscard]] virtual uint32_t read32(uint32_t offset) const = 0;

        /**
         * @brief Returns the total size of this device in bytes.
         * @return Total memory-mapped size.
         */
        [[nodiscard]] virtual uint32_t getSize() const = 0;
    };
} // namespace slowstation::bus
