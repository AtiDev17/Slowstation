#pragma once
#include <cstdint>

namespace slowstation::cpu
{
    /**
     * @class Instruction
     * @brief A utility class to decode MIPS R3000A 32-bit instructions.
     * 
     * This class follows the "Flyweight" pattern: it stores only the 32-bit 
     * instruction value and extracts fields on-demand via bitmasking.
     */
    class Instruction
    {
    public:
        explicit Instruction(const uint32_t value) : m_value(value) {}

        /**
         * @brief Opcode (Primary): Bits [31:26]. 
         * Used to identify J-Type, I-Type, and identify R-Type (if opcode is 0).
         */
        [[nodiscard]] uint8_t opcode() const { return (m_value >> 26) & 0x3F; }

        /**
         * @brief Source Register (rs): Bits [25:21].
         */
        [[nodiscard]] uint8_t rs() const { return (m_value >> 21) & 0x1F; }

        /**
         * @brief Target Register (rt): Bits [20:16].
         */
        [[nodiscard]] uint8_t rt() const { return (m_value >> 16) & 0x1F; }

        /**
         * @brief Destination Register (rd): Bits [15:11]. Used in R-Type.
         */
        [[nodiscard]] uint8_t rd() const { return (m_value >> 11) & 0x1F; }

        /**
         * @brief Shift Amount (shamt): Bits [10:6]. Used in shift operations.
         */
        [[nodiscard]] uint8_t shamt() const { return (m_value >> 6) & 0x1F; }

        /**
         * @brief Function Code (funct): Bits [5:0]. Used to identify sub-opcodes in R-Type.
         */
        [[nodiscard]] uint8_t funct() const { return m_value & 0x3F; }

        /**
         * @brief Immediate/Constant Value (imm): Bits [15:0]. Used in I-Type.
         */
        [[nodiscard]] uint16_t imm() const { return static_cast<uint16_t>(m_value & 0xFFFF); }

        /**
         * @brief Jump Target: Bits [25:0]. Used in J-Type instructions.
         */
        [[nodiscard]] uint32_t target() const { return m_value & 0x03FFFFFF; }

        /**
         * @brief Returns the raw 32-bit word.
         */
        [[nodiscard]] uint32_t value() const { return m_value; }

    private:
        uint32_t m_value;
    };
}
