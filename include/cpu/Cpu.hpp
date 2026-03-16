#pragma once
#include <array>
#include <memory>
#include <cstdint>

// Forward declarations keep compile times fast!
namespace slowstation::bus
{
    class Interconnect;
}

class ILogger;

namespace slowstation::cpu
{
    /**
     * @class Cpu
     * @brief Core MIPS R3000A implementation for the PlayStation emulator.
     */
    class Cpu
    {
    public:
        /**
         * @brief Standard constructor using Dependency Injection for the Bus and Logger.
         */
        Cpu(std::unique_ptr<bus::Interconnect> interconnect, ILogger& logger);

        /**
         * @brief Resets the CPU state to power-on defaults.
         * PC starts at 0xBFC00000 (BIOS reset vector).
         */
        void reset();

        /**
         * @brief Executes a single instruction.
         * Fetches, decodes, and executes.
         * @return The 32-bit instruction that was executed.
         */
        uint32_t step();

        void setReg(uint32_t index, uint32_t value);

    private:
        // The system bus dispatcher
        std::unique_ptr<bus::Interconnect> m_interconnect;

        // Reference to our logger implementation
        ILogger& m_logger;

        // Register file: 32 general-purpose registers.
        // Rule: R0 is always 0.
        std::array<uint32_t, 32> m_regs{};

        // Program Counter (Points to the CURRENT instruction being executed)
        uint32_t m_pc{};
        
        static constexpr uint32_t PC_RESET = 0xBFC00000;
    };
}
