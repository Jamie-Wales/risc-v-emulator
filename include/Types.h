#pragma once
#include <cstdint>

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

  bool use_immediate;
  bool reg_write;
  bool mem_read;
  bool mem_write;
  bool is_jump;
  bool is_jalr;
  BranchFunc branch_type;

  ALUControl alu_op;
};
