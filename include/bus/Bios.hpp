#pragma once
#include <vector>
#include <string>
#include "IMemoryDevice.hpp"

namespace slowstation::bus
{
    /**
     * @class Bios
     * @brief Represents the 512KB BIOS ROM of the PlayStation 1.
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
         * @brief Standard 32-bit read from the BIOS ROM.
         * @param offset The byte offset into the BIOS (0 to 512KB - 4).
         * @return A 32-bit word in Little-Endian format.
         */
        [[nodiscard]] uint32_t read32(uint32_t offset) const override;

        /**
         * @brief Standard 32-bit write. Writing to the BIOS is generally ignored.
         * @param offset The byte offset.
         * @param value The value to write.
         */
        void write32(uint32_t offset, uint32_t value) override;

        /**
         * @brief Returns the BIOS size.
         * @return The size of the BIOS (fixed at 512KB).
         */
        [[nodiscard]] uint32_t getSize() const override;

    private:
        /// Storage for the 512KB BIOS ROM image.
        std::vector<uint8_t> m_data;

        /// Fixed size of the PS1 BIOS ROM.
        static constexpr uint32_t BIOS_SIZE = 512 * 1024; // 512 KB

        /**
         * @brief Loads the BIOS binary file into the internal buffer.
         * @param path The filesystem path to the BIOS image.
         */
        void load(const std::string& path);
    };
} // namespace slowstation::bus
