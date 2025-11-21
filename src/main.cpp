#include "CPU.h"
#include "Cache.h"
#include "Common.h"
#include "Memory.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./riscv_emu <binary_file>" << std::endl;
    return 1;
  }

  RAM ram = RAM(1024 * 1024);
  if (!load_binary(ram, argv[1], 0))
    return 1;

  // L3: 64KB, 8-Way
  Cache l3(&ram, 64 * 1024, 8, "L3");

  // L2: 8KB, 4-Way
  Cache l2(&l3, 8 * 1024, 4, "L2");

  // L1: 1KB, Direct Mapped (1-Way)
  Cache l1(&l2, 1024, 1, "L1");

  CPU cpu(&l1);

  while (true) {
    cpu.step();
  }
}
