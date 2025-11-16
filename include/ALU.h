#pragma once
#include "Types.h"
#include <cstdint>

class ALU {
public:
  static uint32_t execute(uint32_t src_a, uint32_t src_b, ALUControl control_signal);
};
