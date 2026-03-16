#include "bus/Interconnect.hpp"
#include <stdexcept>
#include <iostream>
#include <format>

namespace slowstation::bus
{
    /**
     * @brief Initializes the system bus with BIOS and RAM.
     */
    Interconnect::Interconnect(std::unique_ptr<IMemoryDevice> bios, std::unique_ptr<IMemoryDevice> ram)
        : m_bios(std::move(bios))
        , m_ram(std::move(ram))
    {
    }

    /**
     * @brief Dispatches 32-bit reads to the appropriate hardware device.
     */
    uint32_t Interconnect::read32(const uint32_t address) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM (0x00000000 - 0x001FFFFF)
        if (physical_address < m_ram->getSize())
        {
            return m_ram->read32(physical_address);
        }

        // 2. BIOS ROM (0x1FC00000 - 0x1FC7FFFF)
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            const uint32_t offset = physical_address - 0x1FC00000;
            return m_bios->read32(offset);
        }

        // Error: Access to unmapped memory.
        throw std::out_of_range(std::format("Interconnect: Unhandled read32 at physical address: 0x{:08X}", physical_address));
    }

    /**
     * @brief Dispatches 32-bit writes to hardware or logs MMIO operations.
     */
    void Interconnect::write32(const uint32_t address, const uint32_t value) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM (0x00000000 - 0x001FFFFF)
        if (physical_address < m_ram->getSize())
        {
            m_ram->write32(physical_address, value);
            return;
        }

        // 2. BIOS ROM (Writing to BIOS is generally ignored or handled by flash logic).
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            const uint32_t offset = physical_address - 0x1FC00000;
            m_bios->write32(offset, value);
            return;
        }

        // 3. Memory Control Registers (0x1F801000 - 0x1F801060)
        // [HACK] Ignore these for now to allow BIOS boot to continue.
        if (physical_address >= 0x1F801000 && physical_address <= 0x1F801060) {
             std::cout << std::format("[BUS] Ignoring write32 to MMIO Reg 0x{:08X} (Value: 0x{:08X})\n", physical_address, value);
             return;
        }

        // 4. Cache Control Register (0x1FFE0130)
        if (physical_address == 0x1FFE0130) {
            std::cout << std::format("[BUS] Ignoring write32 to Cache Control Reg 0x{:08X} (Value: 0x{:08X})\n", physical_address, value);
            return;
        }

        // 5. Fatal: Unhandled write access.
        std::cerr << std::format("[BUS] ERROR: Unhandled write32 at physical address: 0x{:08X} (Value: 0x{:08X})\n", physical_address, value) << std::endl;
    }

    /**
     * @brief Masks the top 3 bits to convert virtual MIPS segments to physical addresses.
     */
    uint32_t Interconnect::maskAddress(const uint32_t address)
    {
        // Virtual segments (KSEG0, KSEG1, etc.) map to the same physical space in the PS1.
        return address & 0x1FFFFFFF;
    }
} // namespace slowstation::bus
