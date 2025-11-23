#pragma once
#include "Types.h"
#include <cstdint>

class ALU {
public:
  uint64_t execute(uint64_t src_a, uint64_t src_b, ALUControl control_signal);
};
