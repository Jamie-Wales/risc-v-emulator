#include "Types.h"
#include <ALU.h>

uint32_t ALU::execute(uint32_t src_a, uint32_t src_b,
                      ALUControl control_signal) {
  switch (control_signal) {
  case ALUControl::ADD:
    return src_a + src_b;
  case ALUControl::SUB:
    return src_a - src_b;
  case ALUControl::SLT:
    return ((int32_t)src_a < (int32_t)src_b) ? 1 : 0;
  case ALUControl::SLTU:
    return (src_a < src_b) ? 1 : 0;
  default:
    return 0;
  }
}
