#pragma once
#include "ALU.h"
#include "Memory.h" // User provided Common.h/Memory.h
#include <array>
#include <cstdint>

class CPU {
private:
  uint32_t pc = 0;
  std::array<uint32_t, 32> registers;
  Memory *bus = nullptr;
  ALU alu;

public:
  void connect_bus(Memory *memory_device) { bus = memory_device; }

  uint32_t &x(uint32_t n);
  uint32_t get_pc() const;

  void step();
};
