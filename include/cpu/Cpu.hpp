#pragma once
#include <array>
#include <memory>
#include <cstdint>

/**
 * @namespace slowstation::bus
 * @brief Forward declaration of the memory system components.
 */
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
     *
     * This class implements the R3000A CPU, including its register file,
     * program counter, and execution logic. It follows a "Two-Step PC"
     * architecture to accurately emulate branch delay slots.
     */
    class Cpu
    {
    public:
        /**
         * @brief Standard constructor using Dependency Injection for the Bus and Logger.
         * @param interconnect Pointer to the system's memory-mapped I/O dispatcher.
         * @param logger Reference to the logging interface.
         */
        Cpu(std::unique_ptr<bus::Interconnect> interconnect, ILogger& logger);

        /**
         * @brief Resets the CPU state to power-on defaults.
         *
         * Initializes the PC to the BIOS reset vector (0xBFC00000)
         * and clears all general-purpose and COP0 registers.
         */
        void reset();

        /**
         * @brief Executes a single instruction.
         *
         * Performs the fetch, decode, and execute cycle.
         * @return The 32-bit opcode that was executed.
         */
        uint32_t step();

        /**
         * @brief Safely updates a general-purpose register.
         *
         * Enforces the hardware rule that R0 ($zero) is always zero.
         * @param index The register index (0-31).
         * @param value The 32-bit value to store.
         */
        void setReg(uint32_t index, uint32_t value);

    private:
        /// Pointer to the system's memory-mapped I/O dispatcher.
        std::unique_ptr<bus::Interconnect> m_interconnect;

        /// Reference to the logging interface for diagnostic output.
        ILogger& m_logger;

        /**
         * @brief General Purpose Register (GPR) file.
         * R0 is hardwired to 0; others are for general use.
         */
        std::array<uint32_t, 32> m_regs{};

        /**
         * @brief Coprocessor 0 (COP0) Register file.
         * Used for system control, exception handling, and cache control.
         */
        std::array<uint32_t, 32> m_cop0_regs{};

        /**
         * @brief Current Program Counter (PC).
         * Points to the instruction currently being executed (or in the delay slot).
         */
        uint32_t m_pc{};

        /**
         * @brief Next Program Counter.
         * Points to the next instruction to be fetched into the execution stream.
         */
        uint32_t m_next_pc{};
        
        /// Standard MIPS R3000A Reset Vector address.
        static constexpr uint32_t PC_RESET = 0xBFC00000;
    };
} // namespace slowstation::cpu
