#include <ALU.h>
#include <iostream>

uint32_t ALU::execute(uint32_t src_a, uint32_t src_b,
                      ALUControl control_signal) {
  switch (control_signal) {
  case ALUControl::ADD:
    std::cout << "ADDING " << src_a << " " << src_b << std::endl;
    return src_a + src_b;
  case ALUControl::SUB:
    return src_a - src_b;
  default:
    return 0;
  }
}
