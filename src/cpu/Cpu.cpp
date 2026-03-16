#include "cpu/Cpu.hpp"
#include "logger/ILogger.hpp"
#include "bus/Interconnect.hpp"
#include <format>
#include <utility>

namespace slowstation::cpu
{
    Cpu::Cpu(std::unique_ptr<bus::Interconnect> interconnect, ILogger& logger) 
        : m_interconnect(std::move(interconnect))
        , m_logger(logger)
    {
        // CPU starts in a known state
        reset();
    }

    void Cpu::reset()
    {
        // PS1 BIOS Reset Vector
        m_pc = PC_RESET;

        // Clear all registers. 
        // Note: In Phase 2 implementation, we must ensure m_regs[0] stays 0.
        m_regs.fill(0);

        m_logger.Info(std::format("CPU Reset. PC initialized to 0x{:08X}", m_pc));
    }

    uint32_t Cpu::step()
    {
        // 1. Fetch
        // The interconnect handles the memory mapping (KSEG1 -> Physical BIOS)
        const uint32_t instruction = m_interconnect->read32(m_pc);

        // 2. Log (Hexadecimal is key for debugging opcodes!)
        m_logger.Info(std::format("Fetch 0x{:08X} at PC 0x{:08X}", instruction, m_pc));

        // 3. Increment PC (Instructions are 32-bit / 4-bytes)
        m_pc += 4;

        // 4. Decode & Execute (Next task: MIPS Instruction Set)
        
        return instruction;
    }
}
