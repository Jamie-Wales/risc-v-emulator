#pragma once

#include "ALU.h"
#include "CSR.h"
#include "MMU.h"
#include "Memory.h"
#include "Types.h"
#include <array>
#include <cstdint>
#define DEBUG

class CPU {
private:
  uint64_t pc;
  std::array<uint64_t, 32> registers;
  MMU mmu;
  CSR csr;
  Memory *bus;
  ALU alu;
  PrivilegeMode current_mode = PrivilegeMode::MACHINE;

public:
  void raise_exception(ExceptionCode code, uint64_t tval);
  explicit CPU(Memory *memory);
  void handle_sys_call(uint64_t id);
  void set_pc(uint64_t start_addr);
  uint64_t get_pc() const;
  uint64_t &x(const uint32_t n);
  void step();
};
