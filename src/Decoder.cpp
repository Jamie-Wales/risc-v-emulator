#include "Decoder.h"
#include <iostream>

inline int64_t sext(int32_t val) { return (int64_t)val; }

inline uint32_t funct3(uint32_t value) { return (value >> 12) & 0x7; }
inline uint32_t funct7(uint32_t value) { return (value >> 25) & 0x7F; }

inline int64_t extractITypeImm(uint32_t raw) {
  return sext(((int32_t)raw) >> 20);
}
inline int64_t extractSTypeImm(uint32_t raw) {
  int32_t imm = ((raw >> 25) & 0x7F) << 5 | ((raw >> 7) & 0x1F);
  return sext((imm << 20) >> 20);
}
inline int64_t extractBTypeImm(uint32_t raw) {
  int32_t imm = ((raw >> 31) & 1) << 12 | ((raw >> 7) & 1) << 11 |
                ((raw >> 25) & 0x3F) << 5 | ((raw >> 8) & 0xF) << 1;
  return sext((imm << 19) >> 19);
}
inline int64_t extractUTypeImm(uint32_t raw) {
  return sext((int32_t)(raw & 0xFFFFF000));
}
inline int64_t extractJTypeImm(uint32_t raw) {
  int32_t imm = ((raw >> 31) & 1) << 20 | ((raw >> 12) & 0xFF) << 12 |
                ((raw >> 20) & 1) << 11 | ((raw >> 21) & 0x3FF) << 1;
  return sext((imm << 11) >> 11);
}

inline CSROp extractCsrOp(uint32_t raw) {
  switch (raw) {
  case 1:
    return CSROp::RW;
  case 2:
    return CSROp::RS;
  case 3:
    return CSROp::RC;
  default:
    return CSROp::NONE;
  }
}

ALUControl get_alu_control(uint32_t raw, uint32_t opcode) {
  uint32_t f3 = funct3(raw);
  uint32_t f7 = funct7(raw);
  if (opcode == 0x33 || opcode == 0x13) {
    switch (f3) {
    case 0x0:
      if (opcode == 0x33 && (f7 & 0x20))
        return ALUControl::SUB;
      return ALUControl::ADD;
    case 0x1:
      return ALUControl::SLL;
    case 0x2:
      return ALUControl::SLT;
    case 0x3:
      return ALUControl::SLTU;
    case 0x4:
      return ALUControl::XOR;
    case 0x5:
      if (f7 & 0x20)
        return ALUControl::SRA;
      return ALUControl::SRL;
    case 0x6:
      return ALUControl::OR;
    case 0x7:
      return ALUControl::AND;
    }
  }
  return ALUControl::ADD;
}

DecodedInstruction decode(const uint32_t raw_value) {
  DecodedInstruction instr;
  const uint32_t opcode = raw_value & 0x7F;
  const uint32_t f3 = funct3(raw_value);

  instr.rd_addr = (raw_value >> 7) & 0x1F;
  instr.rs1_addr = (raw_value >> 15) & 0x1F;
  instr.rs2_addr = (raw_value >> 20) & 0x1F;

  switch (opcode) {
  case 0x73: {
    if (f3 == 0) {
      instr.set(Ctrl::IS_ECALL);
    } else {
      instr.set(Ctrl::IS_CSR);
      instr.immediate = extractITypeImm(raw_value);
      switch (f3) {
      case 1:
        instr.csr_op = CSROp::RW;
        break;
      case 2:
        instr.csr_op = CSROp::RS;
        break;
      case 3:
        instr.csr_op = CSROp::RC;
        break;
      default:
        instr.csr_op = CSROp::NONE;
        break;
      }
    }
    break;
  }
  case 0x33:
    instr.set(Ctrl::REG_WRITE);
    instr.alu_op = get_alu_control(raw_value, opcode);
    break;
  case 0x13:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractITypeImm(raw_value);
    instr.alu_op = get_alu_control(raw_value, opcode);
    break;
  case 0x03:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::MEM_READ);
    instr.immediate = extractITypeImm(raw_value);
    switch (f3) {
    case 0x0:
      instr.mem_width = MemWidth::BYTE;
      break;
    case 0x1:
      instr.mem_width = MemWidth::HALF;
      break;
    case 0x2:
      instr.mem_width = MemWidth::WORD;
      break;
    case 0x3:
      instr.mem_width = MemWidth::DOUBLE;
      break;
    case 0x4:
      instr.mem_width = MemWidth::BYTE;
      instr.set(Ctrl::IS_UNSIGNED);
      break;
    case 0x5:
      instr.mem_width = MemWidth::HALF;
      instr.set(Ctrl::IS_UNSIGNED);
      break;
    case 0x6:
      instr.mem_width = MemWidth::WORD;
      instr.set(Ctrl::IS_UNSIGNED);
      break;
    }
    break;
  case 0x23: // STORE
    instr.set(Ctrl::MEM_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractSTypeImm(raw_value);
    switch (f3) {
    case 0x0:
      instr.mem_width = MemWidth::BYTE;
      break;
    case 0x1:
      instr.mem_width = MemWidth::HALF;
      break;
    case 0x2:
      instr.mem_width = MemWidth::WORD;
      break;
    case 0x3:
      instr.mem_width = MemWidth::DOUBLE;
      break;
    }
    break;
  case 0x6F:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_JUMP);
    instr.immediate = extractJTypeImm(raw_value);
    break;
  case 0x67:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_JUMP | Ctrl::IS_JALR);
    instr.immediate = extractITypeImm(raw_value);
    break;
  case 0x37:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM);
    instr.immediate = extractUTypeImm(raw_value);
    instr.rs1_addr = 0;
    break;
  case 0x17:
    instr.set(Ctrl::REG_WRITE | Ctrl::USE_IMM | Ctrl::IS_AUIPC);
    instr.immediate = extractUTypeImm(raw_value);
    break;
  case 0x63:
    instr.immediate = extractBTypeImm(raw_value);
    instr.alu_op = ALUControl::ADD;

    switch (f3) {
    case 0x0:
      instr.branch_type = BranchFunc::BEQ;
      break;
    case 0x1:
      instr.branch_type = BranchFunc::BNE;
      break;
    case 0x4:
      instr.branch_type = BranchFunc::BLT;
      break;
    case 0x5:
      instr.branch_type = BranchFunc::BGE;
      break;
    case 0x6:
      instr.branch_type = BranchFunc::BLTU;
      break;
    case 0x7:
      instr.branch_type = BranchFunc::BGEU;
      break;
    default:
      std::cerr << "Error: Unknown Branch funct3: " << f3 << std::endl;
      instr.branch_type = BranchFunc::NONE;
      break;
    }
    break;
  }
  return instr;
}

const char *get_alu_name(ALUControl op) { return "ALU"; }
const char *get_branch_name(BranchFunc op) { return "BR"; }
