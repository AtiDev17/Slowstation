#include "bus/Interconnect.hpp"

namespace slowstation::bus
{
    /**
     * @brief Constructor using Member Initializer List for efficient ownership transfer.
     * @param bios A unique_ptr to the IMemoryDevice representing the BIOS.
     */
    Interconnect::Interconnect(std::unique_ptr<IMemoryDevice> bios) : m_bios(std::move(bios))
    {
    }

    /**
     * @brief Routes 32-bit read requests to the correct hardware device based on the physical address.
     * 
     * This is the "Master Dispatcher" for the system. It converts virtual addresses 
     * to physical ones and checks address ranges to find the target component.
     * 
     * @param address The 32-bit virtual address requested by the CPU.
     * @return The 32-bit value retrieved from the mapped device.
     * @throws std::out_of_range If the address does not map to any known device.
     */
    uint32_t Interconnect::read32(const uint32_t address) const
    {
        // 1. Convert Virtual Address to Physical Address using 29-bit masking.
        const uint32_t physical_address = maskAddress(address);

        // 2. Check if the address falls within the BIOS ROM physical range (0x1FC00000).
        if (physical_address >= 0x1FC00000 && physical_address < 0x1FC00000 + m_bios->getSize())
        {
            // Calculate the relative offset within the BIOS device.
            const uint32_t offset = physical_address - 0x1FC00000;
            return m_bios->read32(offset);
        }

        // 3. Fallback: Throw an exception for any unhandled memory access (Unhandled Fetch).
        throw std::out_of_range("Interconnect: Unhandled read32 at physical address: 0x" + std::to_string(physical_address));
    }

    /**
     * @brief Masks the top 3 bits of a 32-bit address to translate Virtual to Physical.
     * 
     * The PS1 uses segments (KUSEG, KSEG0, KSEG1). Masking with 0x1FFFFFFF 
     * collapses these segments into the shared 512MB physical address space.
     * 
     * @param address The virtual address to mask.
     * @return The 29-bit physical address.
     */
    uint32_t Interconnect::maskAddress(const uint32_t address)
    {
        return address & 0x1FFFFFFF;
    }
} // slowstation
