#include "ALU.h"

uint64_t ALU::execute(uint64_t src_a, uint64_t src_b,
                      ALUControl control_signal) {
  switch (control_signal) {
  case ALUControl::ADD:
    return src_a + src_b;
  case ALUControl::SUB:
    return src_a - src_b;
  case ALUControl::AND:
    return src_a & src_b;
  case ALUControl::OR:
    return src_a | src_b;
  case ALUControl::XOR:
    return src_a ^ src_b;

  case ALUControl::SLL:
    return src_a << (src_b & 0x3F);
  case ALUControl::SRL:
    return src_a >> (src_b & 0x3F);
  case ALUControl::SRA:
    return (int64_t)src_a >> (src_b & 0x3F);
  case ALUControl::SLT:
    return ((int64_t)src_a < (int64_t)src_b) ? 1 : 0;
  case ALUControl::SLTU:
    return (src_a < src_b) ? 1 : 0;

  default:
    return 0;
  }
}
