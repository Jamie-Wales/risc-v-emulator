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

  const uint32_t val1 = x(d.rs1_addr);
  const uint32_t val2 = (d.use_immediate) ? d.immediate : x(d.rs2_addr);

  uint32_t result = alu.execute(val1, val2, d.alu_op);

#ifdef DEBUG
  std::cout << "[0x" << std::hex << pc << "] ";
  if (d.branch_type != BranchFunc::NONE) {
    std::cout << get_branch_name(d.branch_type) << " ";
    std::cout << "x" << std::dec << d.rs1_addr << "(" << val1 << ") vs ";
    std::cout << "x" << d.rs2_addr << "(" << val2 << ") ";
    std::cout << "Imm:" << std::dec << d.immediate;
  } else if (d.is_jump) {
    std::cout << (d.is_jalr ? "JALR" : "JAL") << " Imm:" << std::dec
              << d.immediate;
  } else if (d.mem_write) {
    std::cout << "SW x" << std::dec << d.rs2_addr << "(" << x(d.rs2_addr)
              << ") ";
    std::cout << "-> [0x" << std::hex << result << "]";
  } else if (d.mem_read) {
    std::cout << "LW x" << std::dec << d.rd_addr << " <- [0x" << std::hex
              << result << "]";
  } else {
    std::cout << get_alu_name(d.alu_op) << " ";
    std::cout << "x" << std::dec << d.rd_addr << " = ";
    std::cout << "x" << d.rs1_addr << "(" << val1 << ") ";
    if (d.use_immediate)
      std::cout << "+ Imm(" << d.immediate << ")";
    else
      std::cout << "op x" << d.rs2_addr << "(" << val2 << ")";
  }
#endif

  if (d.mem_write) {
    const uint32_t data_to_store = x(d.rs2_addr);
    bus->write(result, data_to_store);
  } else if (d.mem_read) {
    result = bus->read(result);
  }

  if (d.reg_write && d.rd_addr != 0) {
    if (d.is_jump) {
      x(d.rd_addr) = pc + 4;
    } else {
      x(d.rd_addr) = result;
    }

#ifdef DEBUG
    std::cout << " -> Res:" << std::dec << x(d.rd_addr);
#endif
  }

  if (d.is_jump) {
    if (d.is_jalr) {
      next_pc = (val1 + d.immediate) & ~1;
    } else {
      next_pc = pc + d.immediate;
    }
  } else if (d.branch_type != BranchFunc::NONE) {
    bool branch_taken = false;
    switch (d.branch_type) {
    case BranchFunc::NONE:
      break;
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
