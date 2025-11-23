#pragma once
#include <cstdint>

namespace Ctrl {
const uint32_t NONE = 0;
const uint32_t REG_WRITE = 1 << 0;   // Writes back to Register
const uint32_t USE_IMM = 1 << 1;     // Uses Immediate instead of RS2
const uint32_t MEM_READ = 1 << 2;    // Load from Memory
const uint32_t MEM_WRITE = 1 << 3;   // Store to Memory
const uint32_t IS_JUMP = 1 << 4;     // Unconditional Jump (JAL/JALR)
const uint32_t IS_JALR = 1 << 5;     // Specific JALR flag
const uint32_t IS_AUIPC = 1 << 6;    // Add Upper Immediate to PC
const uint32_t IS_UNSIGNED = 1 << 7; // Zero-extend loads (LBU, LHU, LWU)
const uint32_t IS_ECALL = 1 << 8;    // Sys call
const uint32_t IS_CSR = 1 << 9;
} // namespace Ctrl

enum class ALUControl { ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU };

enum class BranchFunc { NONE, BEQ, BNE, BLT, BGE, BLTU, BGEU };
enum class MemWidth { BYTE, HALF, WORD, DOUBLE };
enum class CSROp { NONE, RW, RS, RC };

struct DecodedInstruction {
  uint32_t rd_addr;
  uint32_t rs1_addr;
  uint32_t rs2_addr;
  int64_t immediate;

  uint32_t control_signals;
  ALUControl alu_op;
  BranchFunc branch_type;
  MemWidth mem_width;
  CSROp csr_op;

  DecodedInstruction()
      : rd_addr(0), rs1_addr(0), rs2_addr(0), immediate(0), control_signals(0),
        alu_op(ALUControl::ADD), branch_type(BranchFunc::NONE),
        mem_width(MemWidth::WORD), csr_op(CSROp::NONE) {}

  bool has(uint32_t flag) const { return (control_signals & flag) != 0; }

  void set(uint32_t flag) { control_signals |= flag; }
};
