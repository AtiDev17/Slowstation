#pragma once
#include <vector>
#include <string>
#include "IMemoryDevice.hpp"

namespace slowstation::bus
{
    /**
     * @brief Bios - Represents the 512KB BIOS ROM of the PlayStation 1.
     *
     * This class handles loading the BIOS image from disk and provides
     * 32-bit read access to its contents. It implements the IMemoryDevice interface.
     */
    class Bios : public IMemoryDevice
    {
    public:
        /**
         * @brief Constructor that initializes the BIOS and loads the ROM data.
         * @param path The filesystem path to the BIOS image (e.g., SCPH1001.BIN).
         * @throws std::runtime_error if the BIOS fails to load or is an incorrect size.
         */
        explicit Bios(const std::string& path);

        /**
         * @brief Implementation of IMemoryDevice::read32.
         * @param offset The byte offset into the BIOS (0 to 512KB - 4).
         * @return A 32-bit word in Little-Endian format.
         */
        [[nodiscard]] uint32_t read32(uint32_t offset) const override;

        /**
         * @brief Implementation of IMemoryDevice::getSize.
         * @return The size of the BIOS (fixed at 512KB).
         */
        [[nodiscard]] uint32_t getSize() const override;

    private:
        std::vector<uint8_t> m_data;
        static constexpr uint32_t BIOS_SIZE = 512 * 1024; // 512 KB

        /**
         * @brief Private helper to read the BIOS file into the internal buffer.
         * @param path The filesystem path to the BIOS image.
         */
        void load(const std::string& path);
    };
} // namespace slowstation::bus
