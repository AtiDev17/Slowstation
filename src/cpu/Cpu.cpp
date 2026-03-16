#include "cpu/Instruction.hpp"
#include "cpu/Cpu.hpp"
#include "logger/ILogger.hpp"
#include "bus/Interconnect.hpp"
#include <format>
#include <utility>

namespace slowstation::cpu
{
    /**
     * @brief Initializes the CPU with the provided Interconnect and Logger.
     */
    Cpu::Cpu(std::unique_ptr<bus::Interconnect> interconnect, ILogger& logger) 
        : m_interconnect(std::move(interconnect))
        , m_logger(logger)
    {
        reset();
    }

    /**
     * @brief Resets CPU state and starts PC at the BIOS reset vector.
     */
    void Cpu::reset()
    {
        // PS1 BIOS Reset Vector (0xBFC00000)
        m_pc = PC_RESET;
        m_next_pc = m_pc + 4;

        // Clear all general-purpose and Coprocessor 0 registers.
        m_regs.fill(0);
        m_cop0_regs.fill(0);

        m_logger.Info(std::format("CPU Reset. PC initialized to 0x{:08X}", m_pc));
    }

    /**
     * @brief Fetches, decodes, and executes a single instruction.
     */
    uint32_t Cpu::step()
    {
        const uint32_t current_pc = m_pc;

        // 1. Fetch opcode from the memory system.
        const uint32_t fetched_opcode = m_interconnect->read32(current_pc);
        const auto instruction = Instruction(fetched_opcode);

        m_logger.Info(std::format("Fetch 0x{:08X} at PC 0x{:08X}", fetched_opcode, m_pc));

        // 2. Advanced Pipeline: Update PC and Next PC to support branch delay slots.
        m_pc = m_next_pc;
        m_next_pc += 4;

        // 3. Decode & Execute Logic.
        switch (instruction.opcode())
        {
        case 0x00: // SPECIAL (Opcode 0x00)
            switch (instruction.funct())
            {
            case 0x00: // SLL (Shift Left Logical)
                setReg(instruction.rd(), m_regs[instruction.rt()] << instruction.shamt());
                break;
            case 0x08: // JR (Jump Register)
                m_next_pc = m_regs[instruction.rs()];
                break;
            case 0x21: // ADDU (Add Unsigned)
                setReg(instruction.rd(), m_regs[instruction.rs()] + m_regs[instruction.rt()]);
                break;
            case 0x25: // OR
                setReg(instruction.rd(), m_regs[instruction.rs()] | m_regs[instruction.rt()]);
                break;
            case 0x2B: // SLTU (Set Less Than Unsigned)
                setReg(instruction.rd(), (m_regs[instruction.rs()] < m_regs[instruction.rt()]) ? 1 : 0);
                break;
            default:
                m_logger.Error(std::format("Unknown SPECIAL funct: 0x{:02X}", instruction.funct()));
                break;
            }
            break;

        case 0x02: // J (Jump)
            m_next_pc = (m_pc & 0xF0000000) | (instruction.target() << 2);
            break;

        case 0x03: // JAL (Jump and Link)
            setReg(31, m_next_pc); // Save return address ($31 / $ra)
            m_next_pc = (m_pc & 0xF0000000) | (instruction.target() << 2);
            break;

        case 0x05: // BNE (Branch if Not Equal)
            if (m_regs[instruction.rt()] != m_regs[instruction.rs()])
            {
                const int32_t offset = static_cast<int16_t>(instruction.imm()) << 2;
                m_next_pc = m_pc + offset;
            }
            break;

        case 0x08: // ADDI (Add Immediate - Signed)
        case 0x09: // ADDIU (Add Immediate - Unsigned)
            setReg(instruction.rt(), m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm()));
            break;

        case 0x0D: // ORI (OR Immediate)
            setReg(instruction.rt(), m_regs[instruction.rs()] | instruction.imm());
            break;

        case 0x0F: // LUI (Load Upper Immediate)
            setReg(instruction.rt(), static_cast<uint32_t>(instruction.imm()) << 16);
            break;

        case 0x10: // COP0 (Coprocessor 0 Operations)
            switch (instruction.rs())
            {
            case 0x00: // MFC0 (Move FROM Coprocessor 0)
                setReg(instruction.rt(), m_cop0_regs[instruction.rd()]);
                break;
            case 0x04: // MTC0 (Move TO Coprocessor 0)
            {
                uint32_t cop0_reg_index = instruction.rd();
                m_cop0_regs[cop0_reg_index] = m_regs[instruction.rt()];
                m_logger.Trace(std::format("[COP0] Register ${:d} updated to 0x{:08X}", cop0_reg_index, m_cop0_regs[cop0_reg_index]));
                break;
            }
            default:
                m_logger.Error(std::format("Unknown COP0 operation: 0x{:02X}", instruction.rs()));
                break;
            }
            break;

        case 0x23: // LW (Load Word)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            setReg(instruction.rt(), m_interconnect->read32(address));
            break;
        }

        case 0x2B: // SW (Store Word)
        {
            const auto address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            m_interconnect->write32(address, m_regs[instruction.rt()]);
            break;
        }

        default:
            m_logger.Error(std::format("Unknown opcode: 0x{:02X}", instruction.opcode()));
            break;
        }

        return fetched_opcode;
    }

    /**
     * @brief Updates the general-purpose register at the specified index.
     * Ensures that register 0 is always zero.
     */
    void Cpu::setReg(const uint32_t index, const uint32_t value)
    {
        if (index != 0)
        {
            m_regs[index] = value;
            m_logger.Trace(std::format("[CPU] Reg ${:02d} updated to 0x{:08X}", index, value));
        }
    }
} // namespace slowstation::cpu
