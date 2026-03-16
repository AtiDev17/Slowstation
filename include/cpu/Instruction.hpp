#pragma once
#include <cstdint>

namespace slowstation::cpu
{
    /**
     * @class Instruction
     * @brief Utility for decoding MIPS R3000A 32-bit instructions.
     * 
     * This class follows a "Flyweight" design: it stores only the 32-bit 
     * instruction value and extracts specific fields (opcode, rs, rt, etc.)
     * on-demand via bitwise masking and shifting.
     */
    class Instruction
    {
    public:
        /**
         * @brief Constructor that wraps a raw 32-bit opcode.
         * @param value The 32-bit machine code instruction.
         */
        explicit Instruction(const uint32_t value) : m_value(value) {}

        /**
         * @brief Returns the Primary Opcode: Bits [31:26].
         */
        [[nodiscard]] uint8_t opcode() const { return (m_value >> 26) & 0x3F; }

        /**
         * @brief Returns the Source Register (rs): Bits [25:21].
         */
        [[nodiscard]] uint8_t rs() const { return (m_value >> 21) & 0x1F; }

        /**
         * @brief Returns the Target Register (rt): Bits [20:16].
         */
        [[nodiscard]] uint8_t rt() const { return (m_value >> 16) & 0x1F; }

        /**
         * @brief Returns the Destination Register (rd): Bits [15:11].
         */
        [[nodiscard]] uint8_t rd() const { return (m_value >> 11) & 0x1F; }

        /**
         * @brief Returns the Shift Amount (shamt): Bits [10:6].
         */
        [[nodiscard]] uint8_t shamt() const { return (m_value >> 6) & 0x1F; }

        /**
         * @brief Returns the Function Code (funct): Bits [5:0].
         */
        [[nodiscard]] uint8_t funct() const { return m_value & 0x3F; }

        /**
         * @brief Returns the 16-bit Immediate Value (imm): Bits [15:0].
         */
        [[nodiscard]] uint16_t imm() const { return m_value & 0xFFFF; }

        /**
         * @brief Returns the 26-bit Jump Target: Bits [25:0].
         */
        [[nodiscard]] uint32_t target() const { return m_value & 0x03FFFFFF; }

        /**
         * @brief Returns the raw 32-bit instruction value.
         */
        [[nodiscard]] uint32_t value() const { return m_value; }

    private:
        /// The raw 32-bit machine code instruction.
        uint32_t m_value;
    };
} // namespace slowstation::cpu
