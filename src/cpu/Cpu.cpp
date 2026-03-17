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
     * Starts the CPU in its power-on reset state.
     */
    Cpu::Cpu(std::unique_ptr<bus::Interconnect> interconnect, ILogger& logger) 
        : m_interconnect(std::move(interconnect))
        , m_logger(logger)
    {
        reset();
    }

    /**
     * @brief Resets CPU state and starts the program counter at the BIOS reset vector.
     * Clears all general-purpose registers, special math registers (HI/LO), and COP0 state.
     */
    void Cpu::reset()
    {
        // PS1 BIOS Reset Vector (0xBFC00000).
        m_pc = PC_RESET;
        m_next_pc = m_pc + 4;

        // Reset the architectural state.
        m_regs.fill(0);
        m_cop0_regs.fill(0);
        m_hi = 0;
        m_lo = 0;

        m_logger.Info(std::format("CPU Reset. PC initialized to 0x{:08X}", m_pc));
    }

    /**
     * @brief Fetches, decodes, and executes a single 32-bit MIPS instruction.
     * @return The raw 32-bit opcode that was executed.
     */
    uint32_t Cpu::step()
    {
        const uint32_t current_pc = m_pc;

        // 1. Fetch opcode from the memory system via the Interconnect.
        const uint32_t fetched_opcode = m_interconnect->read32(current_pc);
        const auto instruction = Instruction(fetched_opcode);

        m_logger.Info(std::format("Fetch 0x{:08X} at PC 0x{:08X}", fetched_opcode, m_pc));

        // 2. Advanced Pipeline Management:
        // Update the current PC to the delay slot and prepare the next fetch address.
        m_pc = m_next_pc;
        m_next_pc += 4;

        // 3. Instruction Dispatcher.
        switch (instruction.opcode())
        {
        case 0x00: // SPECIAL (Funct-based R-Type instructions)
            switch (instruction.funct())
            {
            case 0x00: // SLL (Shift Left Logical)
                setReg(instruction.rd(), m_regs[instruction.rt()] << instruction.shamt());
                break;
            case 0x08: // JR (Jump Register)
                m_next_pc = m_regs[instruction.rs()];
                break;
            case 0x10: // MFHI (Move From HI)
                setReg(instruction.rd(), m_hi);
                break;
            case 0x11: // MTHI (Move To HI)
                m_hi = m_regs[instruction.rs()];
                break;
            case 0x12: // MFLO (Move From LO)
                setReg(instruction.rd(), m_lo);
                break;
            case 0x13: // MTLO (Move To LO)
                m_lo = m_regs[instruction.rs()];
                break;
            case 0x18: // MULT (Multiply - Signed)
            {
                // Multiplies two 32-bit signed values to produce a 64-bit result.
                int64_t lhs = static_cast<int32_t>(m_regs[instruction.rs()]);
                int64_t rhs = static_cast<int32_t>(m_regs[instruction.rt()]);
                uint64_t res = static_cast<uint64_t>(lhs * rhs);
                m_hi = static_cast<uint32_t>(res >> 32);
                m_lo = static_cast<uint32_t>(res);
                break;
            }
            case 0x19: // MULTU (Multiply - Unsigned)
            {
                uint64_t lhs = m_regs[instruction.rs()];
                uint64_t rhs = m_regs[instruction.rt()];
                uint64_t res = lhs * rhs;
                m_hi = static_cast<uint32_t>(res >> 32);
                m_lo = static_cast<uint32_t>(res);
                break;
            }
            case 0x1A: // DIV (Divide - Signed)
            {
                int32_t lhs = static_cast<int32_t>(m_regs[instruction.rs()]);
                int32_t rhs = static_cast<int32_t>(m_regs[instruction.rt()]);
                // MIPS R3000A handles division by zero and overflow as no-ops/specific values.
                if (rhs == 0) {
                    m_hi = static_cast<uint32_t>(lhs);
                    m_lo = (lhs >= 0) ? 0xFFFFFFFF : 0x00000001;
                } else if (static_cast<uint32_t>(lhs) == 0x80000000 && rhs == -1) {
                    m_hi = 0;
                    m_lo = 0x80000000;
                } else {
                    m_hi = static_cast<uint32_t>(lhs % rhs);
                    m_lo = static_cast<uint32_t>(lhs / rhs);
                }
                break;
            }
            case 0x1B: // DIVU (Divide - Unsigned)
            {
                uint32_t lhs = m_regs[instruction.rs()];
                uint32_t rhs = m_regs[instruction.rt()];
                if (rhs == 0) {
                    m_hi = lhs;
                    m_lo = 0xFFFFFFFF;
                } else {
                    m_hi = lhs % rhs;
                    m_lo = lhs / rhs;
                }
                break;
            }
            case 0x21: // ADDU (Add Unsigned)
                setReg(instruction.rd(), m_regs[instruction.rs()] + m_regs[instruction.rt()]);
                break;
            case 0x24: // AND
                setReg(instruction.rd(), m_regs[instruction.rs()] & m_regs[instruction.rt()]);
                break;
            case 0x25: // OR
                setReg(instruction.rd(), m_regs[instruction.rs()] | m_regs[instruction.rt()]);
                break;
            case 0x26: // XOR
                setReg(instruction.rd(), m_regs[instruction.rs()] ^ m_regs[instruction.rt()]);
                break;
            case 0x27: // NOR
                setReg(instruction.rd(), ~(m_regs[instruction.rs()] | m_regs[instruction.rt()]));
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
            setReg(31, m_next_pc); // Save return address in $ra ($31).
            m_next_pc = (m_pc & 0xF0000000) | (instruction.target() << 2);
            break;

        case 0x04: // BEQ (Branch if Equal)
            if (m_regs[instruction.rs()] == m_regs[instruction.rt()])
            {
                const int32_t offset = static_cast<int16_t>(instruction.imm()) << 2;
                m_next_pc = m_pc + offset;
            }
            break;

        case 0x05: // BNE (Branch if Not Equal)
            if (m_regs[instruction.rs()] != m_regs[instruction.rt()])
            {
                const int32_t offset = static_cast<int16_t>(instruction.imm()) << 2;
                m_next_pc = m_pc + offset;
            }
            break;

        case 0x08: // ADDI (Add Immediate - Signed)
        case 0x09: // ADDIU (Add Immediate - Unsigned)
            setReg(instruction.rt(), m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm()));
            break;

        case 0x0C: // ANDI (AND Immediate)
            // ANDI always zero-extends the immediate value.
            setReg(instruction.rt(), m_regs[instruction.rs()] & instruction.imm());
            break;

        case 0x0D: // ORI (OR Immediate)
            // ORI always zero-extends the immediate value.
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

        case 0x20: // LB (Load Byte - Signed)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            const uint32_t value = static_cast<int8_t>(m_interconnect->read8(address));
            setReg(instruction.rt(), value);
            break;
        }

        case 0x21: // LH (Load Halfword - Signed)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            const uint32_t value = static_cast<int16_t>(m_interconnect->read16(address));
            setReg(instruction.rt(), value);
            break;
        }

        case 0x23: // LW (Load Word)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            setReg(instruction.rt(), m_interconnect->read32(address));
            break;
        }

        case 0x24: // LBU (Load Byte - Unsigned)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            setReg(instruction.rt(), m_interconnect->read8(address));
            break;
        }

        case 0x25: // LHU (Load Halfword - Unsigned)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            setReg(instruction.rt(), m_interconnect->read16(address));
            break;
        }

        case 0x28: // SB (Store Byte)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            m_interconnect->write8(address, static_cast<uint8_t>(m_regs[instruction.rt()]));
            break;
        }

        case 0x29: // SH (Store Halfword)
        {
            const uint32_t address = m_regs[instruction.rs()] + static_cast<int16_t>(instruction.imm());
            m_interconnect->write16(address, static_cast<uint16_t>(m_regs[instruction.rt()]));
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
     * Enforces the hardwired-to-zero rule for register $zero (0).
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
