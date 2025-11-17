#include "CPU.h"
#include "Decoder.h"
#include <iostream>

uint32_t CPU::get_pc() const { return pc; }

uint32_t &CPU::x(const uint32_t n) {
  if (n == 0) {
    static uint32_t zero = 0;
    return zero;
  }
  return registers[n];
}

void CPU::step() {
  const uint32_t raw_inst = bus->read(pc);
  uint32_t next_pc = pc + 4;

  DecodedInstruction d = decode(raw_inst);

  uint32_t val1;
  if (d.has(Ctrl::IS_AUIPC)) {
    val1 = pc;
  } else {
    val1 = x(d.rs1_addr);
  }

  uint32_t val2;
  if (d.has(Ctrl::USE_IMM)) {
    val2 = d.immediate;
  } else {
    val2 = x(d.rs2_addr);
  }

  uint32_t result = alu.execute(val1, val2, d.alu_op);

#ifdef DEBUG
  std::cout << "[0x" << std::hex << pc << "] ";
  if (d.branch_type != BranchFunc::NONE) {
    std::cout << get_branch_name(d.branch_type) << " ";
    std::cout << "x" << std::dec << d.rs1_addr << "(" << val1 << ") vs ";
    std::cout << "x" << d.rs2_addr << "(" << val2 << ") ";
    std::cout << "Imm:" << std::dec << d.immediate;
  } else if (d.has(Ctrl::IS_JUMP)) {
    std::cout << (d.has(Ctrl::IS_JALR) ? "JALR" : "JAL") << " Imm:" << std::dec
              << d.immediate;
  } else if (d.has(Ctrl::MEM_WRITE)) {
    std::cout << "SW x" << std::dec << d.rs2_addr << "(" << x(d.rs2_addr)
              << ") ";
    std::cout << "-> [0x" << std::hex << result << "]";
  } else if (d.has(Ctrl::MEM_READ)) {
    std::cout << "LW x" << std::dec << d.rd_addr << " <- [0x" << std::hex
              << result << "]";
  } else {
    std::cout << get_alu_name(d.alu_op) << " ";
    std::cout << "x" << std::dec << d.rd_addr << " = ";
    std::cout << "x" << d.rs1_addr << "(" << val1 << ") ";
    if (d.has(Ctrl::USE_IMM))
      std::cout << "+ Imm(" << d.immediate << ")";
    else
      std::cout << "op x" << d.rs2_addr << "(" << val2 << ")";
  }
#endif

  if (d.has(Ctrl::MEM_WRITE)) {
    bus->write(result, x(d.rs2_addr));
  } else if (d.has(Ctrl::MEM_READ)) {
    result = bus->read(result);
  }

  if (d.has(Ctrl::REG_WRITE) && d.rd_addr != 0) {
    if (d.has(Ctrl::IS_JUMP)) {
      x(d.rd_addr) = pc + 4;
    } else {
      x(d.rd_addr) = result;
    }
#ifdef DEBUG
    std::cout << " -> Res:" << std::dec << x(d.rd_addr);
#endif
  }

  if (d.has(Ctrl::IS_JUMP)) {
    if (d.has(Ctrl::IS_JALR)) {
      next_pc = (val1 + d.immediate) & ~1;
    } else {
      next_pc = pc + d.immediate;
    }
  } else if (d.branch_type != BranchFunc::NONE) {
    bool branch_taken = false;
    switch (d.branch_type) {
    case BranchFunc::BEQ:
      branch_taken = (val1 == val2);
      break;
    case BranchFunc::BNE:
      branch_taken = (val1 != val2);
      break;
    case BranchFunc::BLT:
      branch_taken = ((int32_t)val1 < (int32_t)val2);
      break;
    case BranchFunc::BGE:
      branch_taken = ((int32_t)val1 >= (int32_t)val2);
      break;
    case BranchFunc::BLTU:
      branch_taken = (val1 < val2);
      break;
    case BranchFunc::BGEU:
      branch_taken = (val1 >= val2);
      break;
    default:
      break;
    }

    if (branch_taken) {
      next_pc = pc + d.immediate;
#ifdef DEBUG
      std::cout << " [TAKEN]";
#endif
    }
  }

#ifdef DEBUG
  std::cout << std::endl;
#endif

  pc = next_pc;
}
