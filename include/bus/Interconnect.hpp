#pragma once
#include <memory>
#include <cstdint>
#include "IMemoryDevice.hpp"

namespace slowstation::bus
{
    /**
     * @class Interconnect
     * @brief The system bus that routes memory access to the correct hardware devices.
     *
     * It translates 32-bit virtual addresses into physical addresses and dispatches
     * read/write operations to the BIOS, RAM, and various Memory-Mapped I/O (MMIO) registers.
     */
    class Interconnect
    {
    public:
        /**
         * @brief Constructor that takes ownership of the system's memory devices.
         * @param bios Ownership of the 512KB BIOS ROM device.
         * @param ram Ownership of the 2MB main system RAM device.
         */
        explicit Interconnect(std::unique_ptr<IMemoryDevice> bios, std::unique_ptr<IMemoryDevice> ram);

        /**
         * @brief Reads an 8-bit byte from the specified virtual address.
         */
        [[nodiscard]] uint8_t read8(uint32_t address) const;

        /**
         * @brief Reads a 16-bit halfword from the specified virtual address.
         */
        [[nodiscard]] uint16_t read16(uint32_t address) const;

        /**
         * @brief Reads a 32-bit word from the specified virtual address.
         * @param address The virtual address to read from.
         * @return The 32-bit word at the specified location.
         */
        [[nodiscard]] uint32_t read32(uint32_t address) const;

        /**
         * @brief Writes an 8-bit byte to the specified virtual address.
         */
        void write8(uint32_t address, uint8_t value) const;

        /**
         * @brief Writes a 16-bit halfword to the specified virtual address.
         */
        void write16(uint32_t address, uint16_t value) const;

        /**
         * @brief Writes a 32-bit word to the specified virtual address.
         * @param address The virtual address to write to.
         * @param value The 32-bit word to store.
         */
        void write32(uint32_t address, uint32_t value) const;

    private:
        /// Ownership of the BIOS ROM device.
        std::unique_ptr<IMemoryDevice> m_bios;

        /// Ownership of the system's Main RAM.
        std::unique_ptr<IMemoryDevice> m_ram;

        /**
         * @brief Masks the virtual address to its physical equivalent.
         * MIPS R3000A segments (KUSEG, KSEG0, KSEG1) are mapped to the same physical space.
         * @param address The 32-bit virtual address.
         * @return The 32-bit physical address.
         */
        [[nodiscard]] static uint32_t maskAddress(uint32_t address);
    };
} // namespace slowstation::bus
