#include "CPU.h"
#include "Decoder.h"
#include <iostream>

CPU::CPU(Memory *memory) : pc(0), bus(memory) { registers.fill(0); }
void CPU::set_pc(uint64_t start_addr) { pc = start_addr; }
uint64_t CPU::get_pc() const { return pc; }

uint64_t &CPU::x(const uint32_t n) {
  if (n == 0) {
    static uint64_t zero = 0;
    return zero;
  }
  return registers[n];
}

void CPU::handle_sys_call(uint64_t id) {
  switch (id) {
  case 93: {
    int exit_code = x(10);
    std::cout << "[Syscall] Program exited with code: " << exit_code
              << std::endl;
    exit(exit_code);
    break;
  }
  case 64: {
    uint64_t fd = x(10);
    uint64_t buf_addr = x(11);
    uint64_t count = x(12);
    if (fd == 1 || fd == 2) {
      for (uint64_t i = 0; i < count; ++i) {
        char c = (char)bus->read8(buf_addr + i);
        std::cout << c;
      }
      std::cout.flush();
      x(10) = count;
    } else {
      std::cerr << "[Syscall] Warning: write to unknown fd " << fd << std::endl;
      x(10) = -1;
    }
    break;
  }
  default:
    std::cerr << "[Syscall] Unimplemented Syscall ID: " << id << std::endl;
    break;
  }
}

void CPU::step() {
  const uint32_t raw_inst = bus->read32(pc);
  uint64_t next_pc = pc + 4;
  DecodedInstruction d = decode(raw_inst);
  uint64_t val1;
  if (d.has(Ctrl::IS_ECALL)) {
    handle_sys_call(x(17));
    pc = next_pc;
    return;
  }
  if (d.has(Ctrl::IS_AUIPC))
    val1 = pc;
  else
    val1 = x(d.rs1_addr);

  uint64_t val2;
  if (d.has(Ctrl::USE_IMM))
    val2 = d.immediate;
  else
    val2 = x(d.rs2_addr);
  uint64_t result = alu.execute(val1, val2, d.alu_op);

#ifdef DEBUG
  std::cout << "[PC:0x" << std::hex << pc << "] "
            << "Inst:0x" << raw_inst;
  if (d.has(Ctrl::REG_WRITE) && d.rd_addr != 0) {
    std::cout << " x" << std::dec << d.rd_addr << " = 0x" << std::hex << result;
  } else if (d.has(Ctrl::MEM_WRITE)) {
    std::cout << " Mem[0x" << std::hex << result << "] <- 0x" << x(d.rs2_addr);
  } else if (d.branch_type != BranchFunc::NONE) {
    std::cout << " Branch Comp: " << std::hex << val1 << " vs " << val2;
  }
  std::cout << std::endl;
#endif
  if (d.has(Ctrl::MEM_WRITE)) {
    uint64_t store_val = x(d.rs2_addr);
    switch (d.mem_width) {
    case MemWidth::BYTE:
      bus->write8(result, (uint8_t)store_val);
      break;
    case MemWidth::HALF:
      bus->write16(result, (uint16_t)store_val);
      break;
    case MemWidth::WORD:
      bus->write32(result, (uint32_t)store_val);
      break;
    case MemWidth::DOUBLE:
      bus->write64(result, store_val);
      break;
    }
  } else if (d.has(Ctrl::MEM_READ)) {
    uint64_t loaded = 0;
    bool is_unsigned = d.has(Ctrl::IS_UNSIGNED);

    switch (d.mem_width) {
    case MemWidth::BYTE:
      loaded = bus->read8(result);
      if (!is_unsigned)
        loaded = (int64_t)(int8_t)loaded;
      break;
    case MemWidth::HALF:
      loaded = bus->read16(result);
      if (!is_unsigned)
        loaded = (int64_t)(int16_t)loaded;
      break;
    case MemWidth::WORD:
      loaded = bus->read32(result);
      if (!is_unsigned)
        loaded = (int64_t)(int32_t)loaded;
      break;
    case MemWidth::DOUBLE:
      loaded = bus->read64(result);
      break;
    }
    result = loaded;
  }

  if (d.has(Ctrl::REG_WRITE) && d.rd_addr != 0) {
    if (d.has(Ctrl::IS_JUMP))
      x(d.rd_addr) = pc + 4;
    else
      x(d.rd_addr) = result;
  }

  if (d.has(Ctrl::IS_JUMP)) {
    if (d.has(Ctrl::IS_JALR))
      next_pc = (val1 + d.immediate) & ~1;
    else
      next_pc = pc + d.immediate;
  } else if (d.branch_type != BranchFunc::NONE) {
    bool taken = false;

    int64_t s1 = (int64_t)val1;
    int64_t s2 = (int64_t)val2;

    switch (d.branch_type) {
    case BranchFunc::BEQ:
      taken = (val1 == val2);
      break;
    case BranchFunc::BNE:
      taken = (val1 != val2);
      break;
    case BranchFunc::BLT:
      taken = (s1 < s2);
      break;
    case BranchFunc::BGE:
      taken = (s1 >= s2);
      break;
    case BranchFunc::BLTU:
      taken = (val1 < val2);
      break;
    case BranchFunc::BGEU:
      taken = (val1 >= val2);
      break;
    default:
      break;
    }

    if (taken) {
      next_pc = pc + d.immediate;
    }
  }

  pc = next_pc;
}
