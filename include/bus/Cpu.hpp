#pragma once
#include <array>
#include <memory>

namespace slowstation::bus
{
    class Interconnect;
}


namespace slowstation::cpu
{
    class Cpu
    {
    public:
        explicit Cpu(std::unique_ptr<bus::Interconnect> m_interconnect);
        void reset();
        uint32_t step();

    private:
        std::unique_ptr<bus::Interconnect> bus;
        std::array<uint32_t, 32> m_regs;
        uint32_t m_pc;
        static constexpr uint32_t PC_RESET = 0xBFC00000;
    };
}
