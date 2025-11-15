#pragma once
#include <cstdint>

enum class ALUControl { ADD, SUB };

struct DecodedInstruction {
  uint32_t rs1_addr;
  uint32_t rs2_addr;
  uint32_t rd_addr;
  int32_t immediate;

  bool use_immediate;
  bool reg_write;
  bool mem_read;
  bool mem_write;
  bool is_branch;

  ALUControl alu_op;
};
