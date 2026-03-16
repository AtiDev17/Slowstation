#pragma once
#include <memory>
#include <cstdint>
#include "IMemoryDevice.hpp"

namespace slowstation::bus
{
    class Interconnect
    {
    public:
        explicit Interconnect(std::unique_ptr<IMemoryDevice> bios);

        [[nodiscard]] uint32_t read32(uint32_t address) const;

    private:
        std::unique_ptr<IMemoryDevice> m_bios;

        [[nodiscard]] static uint32_t maskAddress(uint32_t address);
    };
}
