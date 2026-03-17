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

    uint8_t Interconnect::read8(uint32_t address) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM (0x00000000 - 0x001FFFFF)
        if (physical_address < m_ram->getSize())
        {
            return m_ram->read8(physical_address);
        }

        // 2. BIOS ROM (0x1FC00000 - 0x1FC7FFFF)
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            return m_bios->read8(physical_address - 0x1FC00000);
        }

        // 3. Expansion Region 1 (0x1F000000)
        if (physical_address >= 0x1F000000 && physical_address < 0x1F080000)
        {
            return 0xFF; // Return "No hardware connected"
        }

        // 4. Fallback: Log and return "No hardware" instead of crashing.
        std::cout << std::format("[BUS] Unhandled read8 at physical address: 0x{:08X}, returning 0xFF\n", physical_address);
        return 0xFF;
    }

    uint16_t Interconnect::read16(uint32_t address) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM
        if (physical_address < m_ram->getSize())
        {
            return m_ram->read16(physical_address);
        }

        // 2. BIOS ROM
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            return m_bios->read16(physical_address - 0x1FC00000);
        }

        // 3. Fallback: Log and return "No hardware" instead of crashing.
        std::cout << std::format("[BUS] Unhandled read16 at physical address: 0x{:08X}, returning 0xFFFF\n", physical_address);
        return 0xFFFF;
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

        // 3. Fallback: Log and return "No hardware" instead of crashing.
        std::cout << std::format("[BUS] Unhandled read32 at physical address: 0x{:08X}, returning 0xFFFFFFFF\n", physical_address);
        return 0xFFFFFFFF;
    }

    void Interconnect::write8(uint32_t address, uint8_t value) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM
        if (physical_address < m_ram->getSize())
        {
            m_ram->write8(physical_address, value);
            return;
        }

        // 2. POST Register (0x1F802041)
        if (physical_address == 0x1F802041)
        {
            std::cout << std::format("[POST] BIOS Progress: 0x{:02X}\n", value);
            return;
        }

        // 3. BIOS ROM (Writing to BIOS is generally ignored)
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            m_bios->write8(physical_address - 0x1FC00000, value);
            return;
        }

        std::cout << std::format("[BUS] Ignoring unhandled write8 at physical address: 0x{:08X} (Value: 0x{:02X})\n", physical_address, value);
    }

    void Interconnect::write16(uint32_t address, uint16_t value) const
    {
        const uint32_t physical_address = maskAddress(address);

        // 1. System RAM
        if (physical_address < m_ram->getSize())
        {
            m_ram->write16(physical_address, value);
            return;
        }

        // 2. SPU Range (0x1F801D80 - 0x1F801E00)
        if (physical_address >= 0x1F801D80 && physical_address < 0x1F801F00)
        {
            // SPU is not implemented yet, ignore writes.
            return;
        }

        // 3. BIOS ROM
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            m_bios->write16(physical_address - 0x1FC00000, value);
            return;
        }

        std::cout << std::format("[BUS] Ignoring unhandled write16 at physical address: 0x{:08X} (Value: 0x{:04X})\n", physical_address, value);
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
             return;
        }

        // 4. Cache Control Register (0x1FFE0130)
        if (physical_address == 0x1FFE0130) {
            return;
        }

        // 5. Fatal: Unhandled write access.
        std::cout << std::format("[BUS] Ignoring unhandled write32 at physical address: 0x{:08X} (Value: 0x{:08X})\n", physical_address, value);
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
