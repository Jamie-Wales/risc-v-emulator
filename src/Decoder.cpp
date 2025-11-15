#include "Decoder.h"
#include <iostream>
#include <stdexcept>

inline uint32_t funct3(uint32_t value) { return (value >> 12) & 0x7; }
inline uint32_t funct7(uint32_t value) { return (value >> 25) & 0x7F; }

int32_t extractITypeImm(uint32_t raw_value) {
  return ((int32_t)raw_value) >> 20;
}

ALUControl get_control(uint32_t raw_value, uint32_t opcode) {
  uint32_t f3 = funct3(raw_value);
  uint32_t f7 = funct7(raw_value);

  if (opcode == 0x33) {
    if (f3 == 0x0) {
      return (f7 & 0x20) ? ALUControl::SUB : ALUControl::ADD;
    }
  } else if (opcode == 0x13) { // I-Type (Immediate)
    if (f3 == 0x0) {
      return ALUControl::ADD;
    }
  }

  throw std::runtime_error("Unknown ALU Control");
}

DecodedInstruction decode(uint32_t raw_value) {
  DecodedInstruction instr = {};
  uint32_t opcode = raw_value & 0x7F;

  instr.rd_addr = (raw_value >> 7) & 0x1F;
  instr.rs1_addr = (raw_value >> 15) & 0x1F;
  instr.rs2_addr = (raw_value >> 20) & 0x1F;

  switch (opcode) {
  case 0x33: {
    instr.use_immediate = false;
    instr.reg_write = true;
    instr.alu_op = get_control(raw_value, opcode);
    break;
  }
  case 0x13: {
    instr.use_immediate = true;
    instr.reg_write = true;
    instr.immediate = extractITypeImm(raw_value);
    instr.alu_op = get_control(raw_value, opcode);
    break;
  }
  default:
    std::cout << "Not Implemented Yet" << std::endl;
  }

  return instr;
}
