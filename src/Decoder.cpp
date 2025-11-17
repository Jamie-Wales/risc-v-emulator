#include "Decoder.h"
#include "Types.h"
#include <iostream>
#include <stdexcept>

inline uint32_t funct3(uint32_t value) { return (value >> 12) & 0x7; }
inline uint32_t funct7(uint32_t value) { return (value >> 25) & 0x7F; }

inline int32_t extractITypeImm(uint32_t raw_value) {
  return ((int32_t)raw_value) >> 20;
}

inline int32_t extractSTypeImm(uint32_t raw_value) {
  int32_t imm11_5 = (raw_value >> 25) & 0x7F;
  int32_t imm4_0 = (raw_value >> 7) & 0x1F;

  int32_t imm12 = (imm11_5 << 5) | imm4_0;
  return (imm12 << 20) >> 20;
}

inline int32_t extractUTypeImm(uint32_t raw_value) {
  return (int32_t)(raw_value & 0xFFFFF000);
}

inline int32_t extractBTypeImm(uint32_t raw_value) {
  int32_t bit_12 = (raw_value >> 31) & 0x1;
  int32_t bit_11 = (raw_value >> 7) & 0x1;
  int32_t bits_10_5 = (raw_value >> 25) & 0x3F;
  int32_t bits_4_1 = (raw_value >> 8) & 0xF;
  int32_t imm =
      (bit_12 << 12) | (bit_11 << 11) | (bits_10_5 << 5) | (bits_4_1 << 1);

  return (imm << 19) >> 19;
}

inline int32_t extractJTypeImm(uint32_t raw_value) {
  int32_t bit_20 = (raw_value >> 31) & 0x1;
  int32_t bits_19_12 = (raw_value >> 12) & 0xFF; // Shift 12, Mask 8 bits
  int32_t bit_11 = (raw_value >> 20) & 0x1;
  int32_t bits_10_1 = (raw_value >> 21) & 0x3FF;
  int32_t imm =
      (bit_20 << 20) | (bits_19_12 << 12) | (bit_11 << 11) | (bits_10_1 << 1);

  return (imm << 11) >> 11;
}

BranchFunc decode_branch_type(uint32_t funct3) {
  switch (funct3) {
  case 0x0:
    return BranchFunc::BEQ;
  case 0x1:
    return BranchFunc::BNE;
  case 0x4:
    return BranchFunc::BLT;
  case 0x5:
    return BranchFunc::BGE;
  case 0x6:
    return BranchFunc::BLTU;
  case 0x7:
    return BranchFunc::BGEU;
  default:
    throw std::runtime_error("Invalid Branch Funct3");
  }
}

ALUControl get_control(uint32_t raw_value, uint32_t opcode) {
  const uint32_t f3 = funct3(raw_value);
  const uint32_t f7 = funct7(raw_value);

  if (opcode == 0x33) {
    if (f3 == 0x0) {
      return (f7 & 0x20) ? ALUControl::SUB : ALUControl::ADD;
    }
  } else if (opcode == 0x13) {
    // I-Type (Immediate)
    if (f3 == 0x0) {
      return ALUControl::ADD;
    }
  }

  throw std::runtime_error("Unknown ALU Control");
}

DecodedInstruction decode(const uint32_t raw_value) {
  DecodedInstruction instr = {};
  const uint32_t opcode = raw_value & 0x7F;
  const uint32_t f3 = funct3(raw_value);

  instr.rd_addr = (raw_value >> 7) & 0x1F;
  instr.rs1_addr = (raw_value >> 15) & 0x1F;
  instr.rs2_addr = (raw_value >> 20) & 0x1F;

  switch (opcode) {
  case 0x33: // R-Type (ADD, SUB)
    instr.set(Ctrl::REG_WRITE);
    instr.alu_op = get_control(raw_value, opcode);
    break;

  case 0x13: // I-Type (ADDI)
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractITypeImm(raw_value);
    instr.alu_op = get_control(raw_value, opcode);
    break;

  case 0x03: // LOAD (LW)
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::MEM_READ);
    instr.immediate = extractITypeImm(raw_value);
    instr.alu_op = ALUControl::ADD;
    break;

  case 0x23: // STORE (SW)
    instr.set(Ctrl::MEM_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractSTypeImm(raw_value);
    instr.alu_op = ALUControl::ADD;
    break;

  case 0x6F: // JAL
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_JUMP);
    instr.immediate = extractJTypeImm(raw_value);
    break;

  case 0x67: // JALR
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_JUMP | Ctrl::IS_JALR);
    instr.immediate = extractITypeImm(raw_value);
    break;

  case 0x37: // LUI
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractUTypeImm(raw_value);
    instr.rs1_addr = 0; // Force x0
    instr.alu_op = ALUControl::ADD;
    break;

  case 0x17: // AUIPC
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_AUIPC);
    instr.immediate = extractUTypeImm(raw_value);
    instr.alu_op = ALUControl::ADD;
    break;
  case 0x63:
    instr.branch_type = decode_branch_type(f3);
    instr.immediate = extractBTypeImm(raw_value);
    instr.alu_op = ALUControl::ADD;
    break;

  default:
    std::cout << "Not Implemented Yet" << std::endl;
  }

  return instr;
}
