#pragma once
#include <cstdint>

namespace Ctrl {
constexpr uint16_t NONE = 0;
constexpr uint16_t USE_IMM = 1 << 0;
constexpr uint16_t REG_WRITE = 1 << 1;
constexpr uint16_t MEM_READ = 1 << 2;
constexpr uint16_t MEM_WRITE = 1 << 3;
constexpr uint16_t IS_JUMP = 1 << 4;
constexpr uint16_t IS_JALR = 1 << 5;
constexpr uint16_t IS_AUIPC = 1 << 6;
constexpr uint16_t IS_SYSCALL = 1 << 7;
} // namespace Ctrl

enum class ALUControl { ADD, SUB, SLT, SLTU };
enum class BranchFunc {
  NONE, // Not a branch
  BEQ,  // Equal
  BNE,  // Not Equal
  BLT,  // Less Than Signed
  BGE,  // Greater/Equal Signed
  BLTU, // Less Than Unsigned
  BGEU  // Greater/Equal Unsigned
};

struct DecodedInstruction {
  uint32_t rs1_addr;
  uint32_t rs2_addr;
  uint32_t rd_addr;
  int32_t immediate;

  uint16_t flags = 0;
  BranchFunc branch_type;
  ALUControl alu_op;

  bool has(const uint16_t mask) const { return (flags & mask) != 0; }
  void set(const uint16_t mask) { flags |= mask; }
};
